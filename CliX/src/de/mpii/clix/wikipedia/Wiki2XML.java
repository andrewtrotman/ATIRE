package de.mpii.clix.wikipedia;

import org.xml.sax.*;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import org.xml.sax.SAXException;
import java.io.*;

import org.apache.tools.bzip2.CBZip2InputStream;
import java.util.*;

import de.mpii.clix.wikipedia.WikiHandler.workitem;


public class Wiki2XML {

	private static boolean readTemplates=false;
    private static boolean collectRedirections=true;
    
    public static boolean debugWikiMarkup=false;
    
    static ArrayList<String> inputfile = new ArrayList<String> ();
    
    static String annofile=null;
    
    public static int numThreads=1; // number of converter threads to spawn, default: 1
    
    public static int numFragments=1; // use this to implicitly split the input into distinct fragments (0... numFragments-1); default: 1 fragment
    
    public static int myFragment; // use this to select a fragment to convert (default: fragment 0)
    
    public static String startAtPage="";
    
    public static String redirectionsFile="";
    
    public static String articleFile="";

    public static String catFile="";
    
    public static ArrayList<String> subFolders = new ArrayList<String> ();
    
    public static int inputPos = 0;

//	public static final String templateExpanderURI="http://infao5602:8088/wiki/index.php/Special:ExpandTemplates";
//	public static final String templateExpanderURI="http://localhost:8088/wiki/index.php/Special:ExpandTemplates";
	public static final String templateExpanderURI="http://localhost:18088/wiki/index.php/Special:ExpandTemplates";

	public static void main(String[] argv)
	{
//        inputfile="c:\\projects\\CLiX\\Wikipedia\\input\\enwiki-20081008-pages-articles.xml.bz2";
//        String inputfile="c:\\projects\\clix\\wikipedia\\test.xml";

//		readAnnotations("c:\\projects\\CLiX\\Wikipedia\\input\\yagoanno.txt.bz2");
		
        if (argv.length<1)
        {
            usage();
            System.exit(1);
        }
        
        for (int i=0;i<argv.length;i++)
        {
        	String arg=argv[i];//.toLowerCase();
        	
            if (arg.startsWith("-numthreads="))
            {
            	numThreads=Integer.parseInt(arg.substring("-numthreads=".length()));
            	if (numThreads<=0)
            	{
            		usage(); System.exit(1);
            	}
            }
            else if (arg.startsWith("-numfragments="))
            {
            	numFragments=Integer.parseInt(arg.substring("-numfragments=".length()));
            	if (numFragments<=0)
            	{
            		usage(); System.exit(1);
            	}
            }
            else if (arg.startsWith("-myfragment="))
            {
            	myFragment=Integer.parseInt(arg.substring("-myfragment=".length()));
            	if ((myFragment<0)||(myFragment>=numFragments))
            	{
            		usage(); System.exit(1);
            	}
            }
            else if (arg.startsWith("-startatpage="))
            {
            	startAtPage=arg.substring("-startatpage=".length());
            	if (startAtPage.startsWith("\"")) startAtPage=startAtPage.substring(1);
            	if (startAtPage.endsWith("\"")) startAtPage=startAtPage.substring(0, startAtPage.length()-1);
            	
            	if (startAtPage.length()==0)
            	{
            		usage(); System.exit(1);
            	}
            }else if(arg.startsWith("-subFolders=")){
            	String subFolder = arg.substring("-subFolders=".length());
            	StringTokenizer token = new StringTokenizer(subFolder,"|");
            	while(token.hasMoreElements()){
            		subFolders.add(token.nextToken());
            	}
            }            
            else if (arg.compareTo("-Dmarkup")==0)
            {
            	debugWikiMarkup=true;
            }

            else if (arg.startsWith("-annofile="))
            {
            	annofile=arg.substring("-annofile=".length());
            	WikiHandler.haveLocalAnnotations=true;
        		WikiHandler.readAnnotations(annofile);        		
            }
            
            else if (arg.startsWith("-articlefile="))
            {
            	articleFile=arg.substring("-articlefile=".length());
            	WikiHandler.readArticles(articleFile);
            }
            
            else if (arg.startsWith("-catfile="))
            {
            	catFile=arg.substring("-catfile=".length());
            	WikiHandler.readCategories(catFile);
            }

            else if (arg.startsWith("-redirfile="))
            {
            	redirectionsFile=arg.substring("-redirfile=".length());
            	WikiHandler.readRedirections(redirectionsFile);
            }
            else if (arg.startsWith("-outputdir="))
            {
            	WikiHandler.outputDir=arg.substring("-outputdir=".length()) + File.separator;
            	
            }
            else
            {
                // we need at least one more parameter
                
                if (argv.length<i+1)
                {
                    usage();
                    System.exit(1);
                }
                
                inputfile.add(arg);                
            }
        }
        
        if (articleFile.length()==0) 
        	WikiHandler.articles=Collections.synchronizedMap(new HashMap<String,String>());
        if (redirectionsFile.length()==0) 
        	WikiHandler.redirections=Collections.synchronizedMap(new HashMap<String,String>());
        else
        	collectRedirections=false;
        
        work();
	}

    public static void usage()
    {
        System.out.println("usage: "+Wiki2XML.class.getName()+" [-numthreads=?] [-numfragments=?] [-myfragment=?] [-startatpage=?] [-outputdir=?] [-annofile=?] [-articlefile=?] [-redirfile=?] [-Dmarkup] input");
    }

    private static class watchdog extends Thread
    {
    	public void run()
    	{
    		System.out.println("watchdog thread running");
    		
    		while (true)
			{
        		try
        		{
            		Thread.sleep(60000);    			
        		}
        		catch(Exception e)
        		{        			
        			System.err.println("watchdog thread interrupted.");
        			continue;
        		}
        		
        		for (int i=0;i<handlers.length;i++)
        		{
        			if (handlerthreads[i].isAlive()==false)
        			{
        				System.out.println("[Watchdog] thread "+handlerthreads[i].getId()+" died, restart.");
        				System.err.println("[Watchdog] thread "+handlerthreads[i].getId()+" died, restart.");
        				handlers[i]=new WikiHandler();
        				handlerthreads[i]=new Thread(handlers[i]);
        				handlerthreads[i].start();
        			}
        		}    			
			}
    	}
    }
	static WikiHandler handlers[];
	static Thread handlerthreads[];
	
	public static watchdog watchdog;
	
	public static java.util.concurrent.ArrayBlockingQueue<workitem> queue;
	
	static void work()
	{
        boolean zippedInput=false;   
       
        
        // start handler threads
        
		handlers=new WikiHandler[numThreads];
		handlerthreads=new Thread[numThreads];
		synchronized(handlers)
		{
			queue=new java.util.concurrent.ArrayBlockingQueue<workitem>(500);
			
			for (int i=0;i<numThreads;i++)
			{
				handlers[i]=new WikiHandler();
				handlerthreads[i]=new Thread(handlers[i]);
				handlerthreads[i].start();
			}
		}

		// start watchdog thread
		watchdog=new watchdog();
		watchdog.setDaemon(true);
		watchdog.start();
		
		try
		{
			SAXParserFactory factory = SAXParserFactory.newInstance();
		    SAXParser parser = factory.newSAXParser();
		    WikiHandler handler = new WikiHandler();
		    
		    if (startAtPage.length()!=0) handler.ignoreAll=true;
		    else handler.ignoreAll=false;
		    
			factory.setFeature("http://apache.org/xml/features/validation/schema",true);
		    
            if (readTemplates)
            {
    			System.out.println("Collect templates");
    			
    			TemplateReader temp=new TemplateReader();
    			InputStream stream=null;
    			for(int i=0; i<inputfile.size();i++){
	    	        
	    	        if (zippedInput) stream=new CBZip2InputStream(new FileInputStream(inputfile.get(i)));
	    	        else stream=new BufferedInputStream(new FileInputStream(inputfile.get(i)));
	
	    			parser.parse(stream,temp);
	    			
	    			stream.close();
    			}
            }
            
            if (collectRedirections)
            {
    			System.out.println("Phase 0 - collect redirections and categories");
    			for(int i=0; i<inputfile.size();i++){
    			    if (inputfile.get(i).endsWith("bz2")) zippedInput=true;
    			    FileInputStream fis=new FileInputStream(inputfile.get(i));
	    	        fis.skip(2);
	    	        InputStream stream = null;
	    	        if (zippedInput) 
	    	        	stream=new CBZip2InputStream(fis);
	    	        else 
	    	        	stream=new BufferedInputStream(fis);
	    	        handler.setCurrentFile(i);
	    			handler.reset(0);
	    			parser.parse(stream,handler);	    			
	    			stream.close();
    			}
//              WikiHandler.dumpArticles("articles.txt");
//              
    			WikiHandler.dumpRedirections(handler.outputDir + File.separator + "redirections.txt");
//              
//              System.exit(1);
            }
            
			System.out.println("Phase 1 - generate XML");
			for(int i=0;i<inputfile.size();i++){
			    FileInputStream fis=new FileInputStream(inputfile.get(i));
    	        fis.skip(2);				
		        InputStream stream=null;
		        handler = new WikiHandler();
		        if (zippedInput) 
		        	stream=new CBZip2InputStream(fis);
		        else 
		        	stream=new BufferedInputStream(fis);
		        handler.setCurrentFile(i);
				handler.reset(1);
				parser.parse(stream,handler);
				
				stream.close();
			}
		}
		catch(SAXParseException e)
		{
			System.out.println("SAX parse exception: "+e);
			System.exit(1);
		}
		catch(SAXException e)
		{
			System.out.println("SAX exception: "+e);
			System.exit(1);
		}
		catch(Exception e)
		{
			System.out.println("Exception: "+e);
			e.printStackTrace();
			System.exit(1);
		}
		
		try
		{
			watchdog.interrupt();
		}
		catch(Exception e)
		{}
		
		for (int i=0;i<numThreads;i++)
		{
			queue.add(handlers[i].new workitem(null,null,null,null,null,null,null,0));
		}

	}

	// maintains a single annotation for a page
	
	public static class annotation
	{
		int conceptid;
		double confidence;
		
		public annotation(int id, double c)
		{
			conceptid=id;
			confidence=c;
		}
		
		public int hashCode()
		{
			return Integer.valueOf(conceptid).hashCode();
		}
		
		public boolean equals(Object o)
		{
			if (o instanceof annotation)
			{
				annotation a=(annotation) o;
				return (a.conceptid==conceptid);
			}
			else return false;
		}
		
		public String toString()
		{
			return "id=\""+conceptid+"\" confidence=\""+confidence+"\"";
		}
	}
	
}
