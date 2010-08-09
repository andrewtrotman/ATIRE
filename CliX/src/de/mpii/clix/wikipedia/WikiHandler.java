package de.mpii.clix.wikipedia;

import edu.mit.jwi.*;
import edu.mit.jwi.item.*;

import org.apache.tools.bzip2.CBZip2InputStream;
import org.xml.sax.*;

import java.util.*;
import java.util.concurrent.TimeUnit;
import java.io.*;
import java.sql.*;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

import java.net.*;

import de.mpii.clix.support.*;
import de.mpii.clix.wikipedia.Wiki2XML.annotation;

/*
 * the generated XML does not include some special features of the original markup:
 * - all HTML tags are removed from the output 
 * - all XML entities present in the markup are converted to nonentities
 *   (at least until we know the set of entities used and can import them)
 * - no links to foreign language pages
 * - no links to any non-empty namespaces (INEX2009)
 * - categories are added as special tags and not included in the generated markup
 * - layout attributes in tables are currently ignored (but kept, INEX2009)
 * - there are problems with ' characters that are at the boundary of an
 *   expression in italics or bold. There is no way to determine locally if
 *   the ' belongs to markup or content; whatever we guess here will lead
 *   to problems with some documents.
 */

//public class WikiHandler implements ContentHandler
public class WikiHandler extends DefaultHandler implements Runnable
{
	public static int phase=0; // 0 - collect, 1 - generate
	
	public static String outputDir="/tmp/YAWN3/";
	
	public int currentFile = 0;
	
	private String templateBaseURL="TemplateStore/"; // *relative* to outputDir
		
	private class element
	{
		String tag;
		StringBuffer content;
	}
	
	LinkedList<element> elementStack;

	SAXParser parser;
	DefaultHandler handler;
	
	static int articleCount;
	static int generatedCount;
	static int abortedCount;
	static int invalidCount;
	
	// global properties
	
	final boolean createWikiLinks=true;
	final boolean createWebLinks=true;
	final boolean createImageLinks=true;
	final boolean createHTMLImage=false;
	
	final boolean createRedirections=true;
	
	final boolean createFiles=true;
	
	final boolean expandTemplates=false;
	
	final boolean createEntities=true;
	
	final boolean collectConcepts=false;
	
	final boolean keepOriginalTags=true; // keep whatever XML/HTML tags are present in the markup
	
	final boolean fixOriginalTags=true; // try to fix any XML from the original markup that is not well-formed
	
	final boolean minimizeLabels=false;
	
	final boolean createTags=true; // create tags in the output (otherwise, the output will just be the plan text)
	
	// this affects only intra-wiki links at the moment
	
	final boolean ignoreDeadLinks=false;
	final boolean markDeadLinks=false;
	
	final boolean generateStylesheet=false;
	final boolean xslt=false;
	
	// debugging
	final boolean debugTables=false;
	boolean debugTokens=false;
	boolean debugTemplates=true;

	// initalized by the caller (!)
	static Map<String,String> redirections;
	static Map<String,String> articles;
	
	static public boolean haveLocalAnnotations=true;
	
	static public boolean useArticleIDs=true; // use ids for filenames, not titles
	static public int numDirLevels=1; // how many directory levels do we want (1 in INEX06, 2 in YAWN, 3 in INEX09)
	static public int levelSize=3; // how many characters per level
	static public boolean useSuffixes=true; // build directory prefixes from title prefixes or suffixes
	
    private Database wikiDB;
    private PreparedStatement readWikiFacts;
    private PreparedStatement writePageConceptMapping;
    private PreparedStatement readPageConceptMapping;
    private PreparedStatement readNewConcepts;
    
    final boolean createAnnotations=false;
    final boolean createNewAnnotations=false;
    
    // templates that will be ignored (not expanded and not included in the XML text)
    // note that this applies only to template calls in the page source, not to
    // any templates included within template calls (they are evaluated on the server).
    
    static final String fbT[]=
    {
    	"commons",
    	"wikiquote",
    	"wiktionary",
    	"us-painter-stub",
    	"color box",
    	"china-mil-bio-stub",
    	"stub",
    	"unreferencedsection",
    	"expert-verify",
    	"fact",
    	"sisterlinks",
    	"wikitravel",
    	"original research",
    	"blpsources",
    	"trivia",
    	"external links",
    	"cleanup",
    	"unreferenced",
    	"wvd",
    	"wikibooks",
    	"refimprove",
    	"antibiotics", // buggy tables
    	"notability",
    	"up",
    	"university of the philippines, manila",
    	"football (soccer) chronology",
    	"singles",
    	"extra album cover 2",
    	"manchester united squad",
    	"navboxes",
    	"downsize",
//    	"fb start",
//    	"fb end",
    	"government departments of canada", // includes politics_of_canada
    };

    static final HashSet<String> forbiddenTemplates;
    
    static
    {
    	forbiddenTemplates=new HashSet<String>();
    	for (int i=0;i<fbT.length;i++)
    		forbiddenTemplates.add(fbT[i]);
    }
    
    static HashSet<String> forbiddenTemplateInfixes;
    
    static
    {
    	forbiddenTemplateInfixes=new HashSet<String>();
    	forbiddenTemplateInfixes.add("stub");    	
    }

    static HashSet<String> forbiddenTemplatePrefixes;
    
    static
    {
    	forbiddenTemplatePrefixes=new HashSet<String>();
    	forbiddenTemplatePrefixes.add("commons");    	
    	forbiddenTemplatePrefixes.add("wikisource");
    	forbiddenTemplatePrefixes.add("politics of");
    	forbiddenTemplatePrefixes.add("politics_of");
    	forbiddenTemplatePrefixes.add("football in");
    }

    // templates where we keep only the parameters (as they are specified),
    // but not the template name (used when templates need to be nested)
    
    static HashSet<String> keepOnlyDataTemplates;
    
    static
    {
    	keepOnlyDataTemplates=new HashSet<String>();
    	keepOnlyDataTemplates.add("template group");
    }
    // templates that will be converted into XML code
    
    static HashSet<String> xmlTemplates;
    
    static
    {
    	xmlTemplates=new HashSet<String>();
    	xmlTemplates.add("protein");
    	xmlTemplates.add("tracklist");
    	xmlTemplates.add("us state");
    	xmlTemplates.add("pharaoh infobox");
    }
    
    // prefixes of template names that will be converted into XML code
    
    static HashSet<String> xmlTemplatePrefixes;
    static
    {
    	xmlTemplatePrefixes=new HashSet<String>();
    	xmlTemplatePrefixes.add("infobox");
    	xmlTemplatePrefixes.add("geobox");
    }
    
    // templates that will be expanded once by their definition and then reconsidered by expandTemplates
    // (this allows to xmlify infoboxes that are encapsulated into another template)
    
    static HashSet<String> expandOnce;
    static
    {
    	expandOnce=new HashSet<String>();
    }
    public void setCurrentFile(int i){
    	currentFile = i;
    }
    // templates that will be handled by the converter, not forwarded to the server
  
    // (currently broken)
    static HashSet<String> parserTemplates;
    static
    {
    	parserTemplates=new HashSet<String>();
//    	parserTemplates.add("IPA");
//    	parserTemplates.add("Link FA");
    }
    // page-to-concept mapping
    
    HashMap<String,List<String>> concepts;
    
//    net.didion.jwnl.dictionary.Dictionary dict;
//    PointerUtils putils;

    IDictionary dict;
    
	public WikiHandler()
	{
		elementStack=new LinkedList<element>();
		categories=new HashSet<String>();
	
		try
		{
			SAXParserFactory factory = SAXParserFactory.newInstance();
		    parser = factory.newSAXParser();
		    handler = new DefaultHandler();
		}
		catch(Exception e)
		{
			problem(e.toString());
		}

		if (haveLocalAnnotations==false)
		{
	        wikiDB=new Database();
	        wikiDB.openDatabase("test", "test", "mpiat5316", "destiny.mpi");
	        
	        readWikiFacts=wikiDB.prepareStatement("select arg2 from test.facts where arg1=?");
	        
	        writePageConceptMapping=wikiDB.prepareStatement("insert into wikipedia.conceptmap(pagetitle,concept,conceptid) values(?,?,?)");
	        
	        readPageConceptMapping=wikiDB.prepareStatement("select concept,conceptid,confidence from wikipedia.conceptmapinex2009 where pagetitle=?");
	        
	        readNewConcepts=wikiDB.prepareStatement("select concept,max(confidence),count(distinct lineage) from newfacts3 where url=? group by concept");

        // test the new, cleaned facts
//        readNewConcepts=wikiDB.prepareStatement("select concept,confidence,0 from cleanedfacts where url=?");
		}
		
        concepts=new HashMap<String,List<String>>();
        
        try
        {
        	//URL u=new URL("file",null,"c:\\projects\\clix\\wordnet\\dict");
        	URL u=new URL("file",null,"/usr/share/wordnet-3.0/dict");
        	dict = new edu.mit.jwi.Dictionary(u);
        	dict.open();
//            JWNL.initialize(new FileInputStream("file_properties.xml"));
        }
        catch(Exception e)
        {
            System.out.println("JWNL init failed:\n"+e);
        }

//        dict=net.didion.jwnl.dictionary.Dictionary.getInstance();
//        putils=PointerUtils.getInstance();
        
        initIncompatibilities();
	}
	
	int cnt=0;
	
	public void characters(char[] text, int start, int length) throws SAXException
	{
//		System.out.println("\ttext: \""+new String(text).substring(start,start+length)+"\" "+length+" bytes");
		
		element el=elementStack.getFirst();
//		if (el.content!=null) el.content.append(new String(text).substring(start,start+length));
//		else el.content=new StringBuffer(new String(text).substring(start,start+length));

		if (el.content!=null) el.content.append(text,start,length);
		else el.content=new StringBuffer(new String(text).substring(start,start+length));

//		cnt++;
//		if (cnt%1000==0)
//			System.out.println("["+cnt+"] content now "+el.content.length()+" bytes");
	}

	public void endDocument() throws SAXException
	{
		if (articleCount%100 != 0) 
			status();
		
		if (phase==0)
			System.out.println("collected "+redirections.size()+" redirections\n");
	}

	boolean ignoreAll=false;
	
	public void endElement(String namespaceURI, String localName, String qualifiedName) throws SAXException
	{
		//System.out.println("endElement("+namespaceURI+","+localName+","+qualifiedName+")");

		element el=elementStack.removeFirst();
		
		if (el.content!=null)
		{
//			// debug
//			if ((phase==1) && currentID != null && currentID.equals("1010"))
//				System.err.println("I got you!");
			
			if (el.tag.equals("text"))
			{
				// it does not make sense to convert these articles as they contain partial markup,
				// invalid markup, or do not work for some other, unknown reason
				// (and frequently make the converter break or stuck)
				
				// this is the list for the enwiki-20081008 dump
				// (the bugs are usually fixed with the new dump, so we may have to change this
				// list in the future)
				
				if ((ignoreAll)&&(currentTitle.compareTo(Wiki2XML.startAtPage)==0))
				{
//					buggyArticle=true;
					ignoreAll=false;
				}
				else if (currentTitle.startsWith("Ackermann function"))
					buggyArticle=true;
				else if (currentTitle.startsWith("Arithmetic-geometric mean"))
					buggyArticle=true;
				else if (currentTitle.startsWith("Bornholm"))
					buggyArticle=true;
				
				else if (currentTitle.startsWith("Binomial distribution"))
					buggyArticle=true;
				else if (currentTitle.startsWith("Boltzmann distribution"))
					buggyArticle=true;
				else if (currentTitle.startsWith("Bertrand Russell"))
					buggyArticle=true;
				else if (currentTitle.startsWith("Big Dig (Boston, Massachusetts)"))
					buggyArticle=true;
//				else if (currentTitle.startsWith("Wheeling Jesuit University"))
//					buggyArticle=true;
//				else if (currentTitle.startsWith("Linaria"))
//					buggyArticle=true;
//				else if (currentTitle.startsWith("William Hall"))
//					buggyArticle=true;

				else if (currentTitle.startsWith("Template:"))
					buggyArticle=true;
				else if (currentTitle.startsWith("Image:"))
					buggyArticle=true;
				else if (currentTitle.startsWith("Wikipedia:"))
					buggyArticle=true;
				else if (currentTitle.startsWith("Category:"))
					buggyArticle=true;

				if (ignoreAll==false)
				{
                    // we have to scan the article text to find the category information
                    
					if (phase==0)
					{
						testRedirection(el.content.toString());
					}
					if ((buggyArticle==false)&&((phase==1)||(collectConcepts)))
						currentContent=el.content.toString();
					else 
						currentContent="";
				}
			}
			else if (el.tag.equals("title"))
			{
				currentTitle=el.content.toString();
//				if (phase==1) 
//                    System.out.println("\n***\nnew document: "+currentTitle);
				//if ((phase==1)&&(currentTitle.equals("April"))) debugTemplates=true;
//				if ((phase==1)&&(currentTitle.equals("Aramaic alphabet"))) debugTokens=true;
//				else debugTokens=false;
			}
			else if (el.tag.equals("timestamp"))
			{
				currentTimestamp=XML.XMLify2(el.content.toString());
				//if (phase==1) System.out.println(currentTimestamp);
			}
			else if (el.tag.equals("id"))
			{
				if (elementStack.size()==2)
					currentID=XML.XMLify2(el.content.toString());
				else if (elementStack.size()==3)
					currentRevisionID=XML.XMLify2(el.content.toString());
				else if (elementStack.size()==4)
					currentAuthorID=XML.XMLify2(el.content.toString());
			}
			else if (el.tag.endsWith("username"))
			{
				currentAuthor=XML.XMLify2(el.content.toString());
			}
			else if (el.tag.equals("page"))
			{
				if (phase==0)
				{
					articles.put(currentTitle, currentID);
				}
				else if ((phase==1)&&(ignoreAll==false)&&(buggyArticle==false)&&(articleCount%Wiki2XML.numFragments==Wiki2XML.myFragment))
				{
					// debug
//					if (currentID.equals("1010"))
//						System.err.println("I got you!");
//					if (articleCount<1648700)
//					{
//						articleCount++; return;
//					}
					workitem w=new workitem(currentTitle,currentID, currentTimestamp,currentAuthor, currentAuthorID, currentContent,currentRevisionID,currentFile);
					currentContent = null;
					while (true)
					{
						try
						{
							if (Wiki2XML.queue.offer(w,1,TimeUnit.DAYS)) 
								break;
						}
						catch(Exception e)
						{
							e.printStackTrace();
						}
						System.err.println("[main thread] queue at capacity limit even after really long time, probably something is broken.");
						System.exit(1);
					}
				} 
				else {
					problem("This page was not processed");
				}
					
				buggyArticle=false;
				
				articleCount++;
				if (articleCount%100==0) status();
                
//                if (generatedCount>10000) System.exit(1);
			}
		}
	}

	boolean lastArticleWasBuggy=false;
	
	static private String hostname="localhost";

	static
	{
		try
		{
			java.net.InetAddress localMachine = java.net.InetAddress.getLocalHost();	
			hostname=localMachine.getHostName();
			if (hostname.indexOf('.')!=-1) hostname=hostname.substring(0,hostname.indexOf('.'));
		}
		catch(java.net.UnknownHostException uhe)
		{
			//handle exception
		}
	}
	
	private void dumpAndResetArticle()
	{
		lastArticleWasBuggy=false;
		
		StringBuffer xml=new StringBuffer();

		// disabled for the time being
        if ((false)&&(phase==0)&&(collectConcepts))
        {
            collectConcepts(categories);
        }
        
        else if (phase==1)
		{
        	HashSet<label> labels=readAndConsolidateLabels(currentTitle);
            LinkedList<label> categoryTags=new LinkedList<label>(readAndConsolidateCategoryLabels(categories,labels)); 
            
			if (createTags)
			{
				xml.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
  				   .append("<!-- generated by CLiX/Wiki2XML [MPI-Inf, MMCI@UdS] $LastChangedRevision: 93 $ on "+Utils.getCurrentDate()+"["+hostname+"] -->\n");
			}
	
			if ((generateStylesheet)&&(currentTitle!=null))
			{
				if (xslt) 
					xml.append(xsltLink());
				else	  
					xml.append(cssLink());
			}
			
			if ((createEntities)&&(createTags))
				xml.append(dtdLink());
			
			if (isRedirect==false)
			      openHeaderTagLn(xml,WikiConstants.articleTag,"xmlns:xlink=\""+XML.nsXLink+"\"");
			else
			      emptyHeaderTagLn(xml,"redirect", "xmlns=\"http://www.mpi-inf.mpg.de/INEX/Wikipedia\" xmlns:xlink=\""+XML.nsXLink+"\"");

//            ListIterator<String> cit=categoryTags.listIterator();
//            for (int i=0;i<categoryTags.size();i++)
//                openTagLn(xml,cit.next());

			if (createAnnotations) 
				openAnnotationTags(categoryTags,xml,true);
            openTagLn(xml,WikiConstants.headerTag);
            
			if (currentTitle!=null)
			{
				openTag(xml,WikiConstants.articleTitleTag);
				xml.append(XML.XMLify(currentTitle));
//				xml.append(currentTitle); // comes from an XML file, should not need XMLification
				closeTagLn(xml,WikiConstants.articleTitleTag);
			}
			
			if (currentID!=null)
			{
				openTag(xml,WikiConstants.articleIDTag);
				xml.append(currentID);
				closeTagLn(xml,WikiConstants.articleIDTag);
			}
			
			if ((currentTimestamp!=null)||(currentRevisionID!=null)||(currentAuthor!=null)||(currentAuthorID!=null))
			{
				openTagLn(xml,WikiConstants.articleRevisionTag);
				
				if (currentRevisionID!=null)
				{
					openTag(xml,WikiConstants.articleRevisionIDTag);
					xml.append(currentRevisionID);
					closeTagLn(xml,WikiConstants.articleRevisionIDTag);
				}
				
				if (currentTimestamp!=null)
				{
					openTag(xml,WikiConstants.articleRevisionTimestampTag);
					xml.append(currentTimestamp);
					closeTagLn(xml,WikiConstants.articleRevisionTimestampTag);
				}
				
				if ((currentAuthor!=null)||(currentAuthorID!=null))
				{
					openTagLn(xml,WikiConstants.articleRevisionContributorTag);
					if (currentAuthor!=null)
					{
						openTag(xml,WikiConstants.articleRevisionContributorUsernameTag);
						xml.append(XML.XMLify(currentAuthor));
						closeTagLn(xml,WikiConstants.articleRevisionContributorUsernameTag);
					}
					if (currentAuthorID!=null)
					{
						openTag(xml,WikiConstants.articleRevisionContributorIDTag);
						xml.append(currentAuthorID);
						closeTagLn(xml,WikiConstants.articleRevisionContributorIDTag);
					}
					closeTagLn(xml,WikiConstants.articleRevisionContributorTag);
				}
				
				closeTagLn(xml,WikiConstants.articleRevisionTag);
			}			
			if (categories.size()>0)
			{
				openTagLn(xml,"categories");
				Iterator<String> it=categories.iterator();
				while (it.hasNext())
				{
					// we need to encode everything, including '&' characters (!)
					openTag(xml,"category");
					xml.append(XML.XMLify(it.next()));
					closeTagLn(xml,"category");
				}
				closeTagLn(xml,"categories");
			}
	
            closeTagLn(xml,WikiConstants.headerTag);
            
			if (currentContent!=null)
			{
				if (isRedirect)
					xml.append(currentContent+"\n");
				else
				{
					openTagLn(xml,WikiConstants.bodyTag);
					xml.append(currentContent);
					closeTagLn(xml,WikiConstants.bodyTag);
				}
			}
			
//            cit=categoryTags.listIterator(categoryTags.size());
//            for (int i=0;i<categoryTags.size();i++)
//                closeTagLn(xml,cit.previous());

			if (createAnnotations) 
				closeAnnotationTags(categoryTags,xml);

			if (isRedirect==false) 
				closeTagLn(xml,WikiConstants.articleTag);
			else				   
				xml.append("</redirect>");
		
			if (((isRedirect==false)/*||((isRedirect)&&(createRedirections))*/)&&(testXML(xml.toString())==false))
			{
				System.out.println("["+articleCount+"] "+currentTitle+(currentTimestamp!=null?"["+currentTimestamp+"]":"")+": generated XML is probably buggy, skip this.");
				invalidCount++;
				buggyArticle=true;
				lastArticleWasBuggy=true;
				dumpXMLFile(useArticleIDs?currentID:currentTitle, currentTitle, xml.toString(),true);
				//System.out.println(contentStore);
			}
			else if (buggyArticle)
			{
//				System.out.println("skip "+currentTitle+" as it seems to be buggy.");
				abortedCount++;
			}
	
			if ((buggyArticle==false)&&(createFiles))
			{
				if ((isRedirect==false)/*||((isRedirect)&&(createRedirections))*/) 
					dumpXMLFile(useArticleIDs?currentID:currentTitle, currentTitle, xml.toString(),false);
			}
		}	
		currentTitle=null;
		currentAuthor=null;
		currentAuthorID=null;
		currentContent=null;
		currentID=null;
		currentRevisionID=null;
		currentTimestamp=null;
		
		categories=new HashSet<String>();

		buggyArticle=false;
		
		isRedirect=false;
	}

//	public void endPrefixMapping(String arg0) throws SAXException {
//		// TODO Auto-generated method stub
//		
//	}
//
//	public void ignorableWhitespace(char[] arg0, int arg1, int arg2) throws SAXException {
//		// TODO Auto-generated method stub
//		
//	}
//
//	public void processingInstruction(String arg0, String arg1) throws SAXException {
//		// TODO Auto-generated method stub
//		
//	}
//
//	public void setDocumentLocator(Locator arg0) {
//		// TODO Auto-generated method stub
//		
//	}
//
//	public void skippedEntity(String arg0) throws SAXException {
//		// TODO Auto-generated method stub
//		
//	}
//
//	public void startDocument() throws SAXException {
//		
//	}

	String currentXML=null;
	String currentTitle=null;
	String currentContent=null;
	String currentID=null;
	String currentTimestamp=null;
	String currentRevisionID=null;
	String currentAuthorID=null;
	String currentAuthor=null;
	
	boolean isRedirect=true;
	
	public void startElement(String namespaceURI, String localName, String qualifiedName, Attributes arg3) throws SAXException
	{
		//System.out.println("startElement("+namespaceURI+","+localName+","+qualifiedName+")");
		
		// if there is already content available from the current element, output it to the target file
		
		// create a new element and put it on the stack
		
		element el=new element();
		el.tag=qualifiedName;
		
		elementStack.addFirst(el);
	}

//	public void startPrefixMapping(String arg0, String arg1) throws SAXException {
//		// TODO Auto-generated method stub
//		
//	}

	// replace invalid characters with '_' (ignoring possible duplicates)
	// and add "$" to any uppercase character (but the first one)

	private String cleanInternalLink(String text2clean) {

		if (text2clean.startsWith(".."))
			text2clean="dot2_"+text2clean;
		else if (text2clean.startsWith("."))
			text2clean="dot_"+text2clean;

		char[] ca = text2clean.replaceAll("\\?|\"|\'|:|&|\\*|\\s", "_").toCharArray();
		StringBuffer newText= new StringBuffer();

		// handling of the first character
		
		if ((Character.isLetter(ca[0])==false)&&(Character.isDigit(ca[0])==false)) newText.append("wiki_").append(ca[0]);
		else newText.append(Character.toUpperCase(ca[0]));
		
		// handling of the remaining characters
		
		for (int i=1; i< ca.length;i++) {
			newText.append(ca[i]);
            if (Character.isUpperCase(ca[i])) newText.append("$");	
		}

		return UTF82ASCII.encode(newText.toString());
	}
	
	private String getPrefix(String wikiname) {
		if (wikiname.length() < 2)
			return wikiname + "_";
		else
		{
			if (wikiname.length()>2)
			{
				if (wikiname.charAt(2)=='$')
					return wikiname.substring(0, Math.min(3,wikiname.length()));
				else if (wikiname.charAt(2)=='%')
					return wikiname.substring(0, Math.min(4,wikiname.length()));
				else
					return wikiname.substring(0, 2);
			}
			else return wikiname.substring(0, 2);
		}
	}

	// from-scratch implementation of text-to-xml converter using string patterns
	
	String contentStore;
	
	private String makeXML(String content)
	{
//		System.out.println(content.substring(0,10).toLowerCase());
		if ((content.length()>=10)&&(content.substring(0,10).toLowerCase().startsWith("#redirect")))
		{
			isRedirect=true;
			
			if (createRedirections)
			{			
		        StringTokenizer tokenizer = new StringTokenizer(content,"='[]{}|*\n!+-#:", true);
		        while (tokenizer.hasMoreTokens())
		        {
		        	String token=tokenizer.nextToken();
		        	if (token.equals("["))
		        	{
		        		if (tokenizer.hasMoreTokens())
		        		{
		        			token=tokenizer.nextToken();
		    	        	if (token.equals("["))
		    	        	{
		    	        		if (tokenizer.hasMoreTokens())
		    	        		{
		    	        			token=tokenizer.nextToken();
		    		        		// we have found the start for the link
		    		        		return handleRedirection(token,tokenizer);
		    	        		}
		    	        	}
		        		}
		        	}
		        }
			}
			else
				return "";
		}

//        System.out.println(content);

//		content=removeComments(content);


		content=removeOnlyComments(content);

		// this removes <nowiki> and <math>
		content=fixSomeHTMLTags(content,true);

        if (Wiki2XML.debugWikiMarkup)
       	{
            System.out.println("***\n***before expanding templates\n***");
        	System.out.println(content);
       	}

		// replace templates by their definition
		if ((expandTemplates))
			content=handleTemplates(content);

		while (content.indexOf("&amp;")!=-1)
			content=XML.unXMLify(content);
		
		content=removeOnlyComments(content);

		content=fixSomeHTMLTags(content,false);

        if (Wiki2XML.debugWikiMarkup)
       	{
            System.out.println("***\n***after expanding templates\n***");
        	System.out.println(content);
       	}
        
		content=fixHTMLTags(content,true);

    	// experimental
    	// remove any spaces at the beginning of a line, should hopefully fix some problems with table cells
    	// that do not start at the beginning of a line
    	int len=0;
    	do
    	{
       	len=content.length();
    	content=content.replaceAll("\n ", "\n");
    	content=content.replaceAll("\n\t", "\n");
    	} while (content.length()!=len);
    	

        if (Wiki2XML.debugWikiMarkup)
       	{
            System.out.println("***\n***before converting markup\n***");
        	System.out.println(content);
       	}

        StringBuffer res=new StringBuffer();
        
        handleContent(content,0,content.length(),res);

        cleanupHeaders(res);

        content=res.toString();
        res.setLength(0);

//		System.out.println("after handleContent()");
//		System.out.println(content);
//		t=content.split("&");
//		System.out.println("&: "+t.length);
//		t=content.split("&amp;");
//		System.out.println("&amp: "+t.length);
     
        if (templates!=null && templates.size()>0)
        {
//        	fixTemplates(content,res);
        	embedTemplates(content,res);
            content=res.toString();
            res.setLength(0);
        }

        if ((references.size()>0)||(content.indexOf(CLIX_REFERENCE_LIST)!=-1))
        {
        	fixReferences(content,res);
            content=res.toString();            
            res.setLength(0);
        }
        
        if (nowikis.size()>0)
        {
        	fixNowikis(content,res);
            content=res.toString();
            res.setLength(0);
       	}

        if (imagemaps.size()>0)
        {
        	fixImageMaps(content,res);
            content=res.toString();
            res.setLength(0);
       	}

        if (maths.size()>0)
        {
        	fixMaths(content,res);
            content=res.toString();
            res.setLength(0);
       	}

        if (cites.size()>0)
        {
        	fixCites(content,res);
            content=res.toString();
            res.setLength(0);
       	}

//		System.out.println("after fixReferences()");
//		t=content.split("&");
//		System.out.println("&: "+t.length);
//		t=content.split("&amp;");
//		System.out.println("&amp: "+t.length);
		
		content=createEntities(content);
		
//		System.out.println("after createEntities()");
//		t=content.split("&");
//		System.out.println("&: "+t.length);
//		t=content.split("&amp;");
//		System.out.println("&amp: "+t.length);

//        System.out.println(content);

        return content;
	}
	
	private boolean buggyArticle=false;
	
	private HashSet<String> categories;
	

	// removed, see revision 91 for the last (nonfunctional) version
	private String handleRedirection(String token, StringTokenizer tokenizer)
	{	
		return "<error>*** unsupported call to handleRedirection! ***"; // this will break any generated xml
	}


	private String makeWikiURL(String title)
	{
		if (title==null) title="";
		
		title=title.trim();
		if (redirections.containsKey(title))
		{
			String newtitle=redirections.get(title);
			if (newtitle.length()>0) title=newtitle;
//			System.out.println("replace link: ["+title+"] -> ["+redirections.get(title)+"]");
//			title=redirections.get(title);
		}
		return createFileName(title, title,true,currentTitle,false,false);
	}


	private String makeImageURL(String title)
	{
		return title.replaceAll("\\s","_");
	}

	private static String filenamepatch="";
	
	private static String climbDirs="";
	
	static
	{
		String tmp="";
		for (int i=0;i<levelSize;i++)
		{
			if (useArticleIDs)
				tmp+="0";
			else
				tmp+="_";
			
		}
		for (int i=0;i<numDirLevels;i++)
		{
			if (useArticleIDs)
				filenamepatch+=tmp;
			else
				filenamepatch+=tmp;
			
			climbDirs+="../";
		}
	}
	// this is a universal method to create filenames in hierarchical directories
	// it can be configured using global parameters
	
	// relative links have been disabled
	
	private String createFileName(
			String filename,
			String title,
			boolean relative,
			String owntitle,
			boolean buggy,
			boolean create)
	{
		if (filename.length()==0) return "";
		
		String dir="";
		String subFolder = "";
		if(Wiki2XML.subFolders != null && Wiki2XML.subFolders.size() != 0){
			subFolder = Wiki2XML.subFolders.get(currentFile);
		}
		String lcname=title.toLowerCase();
		if (!relative)
		{
			if (lcname.startsWith("image:"))
			{
				filename=filename.substring(filename.indexOf(':') + 1);
				dir=outputDir+"images/";
			}
			else if (lcname.startsWith("template:") || lcname.startsWith("模板:"))
			{
				filename=filename.substring(filename.indexOf(':') + 1);
				dir=outputDir+"templates/";
			}
			else if (lcname.startsWith("wikipedia:"))
			{
				filename=filename.substring(filename.indexOf(':') + 1);
				dir=outputDir+"wikipedia/";
			}
			else if (lcname.startsWith("category:") || lcname.startsWith("分类:"))
			{
				filename=filename.substring(filename.indexOf(':') + 1);
				dir=outputDir+"categories/";
			}
			else if (lcname.startsWith("帮助:"))
			{
				filename=filename.substring(filename.indexOf(':') + 1);
				dir=outputDir+"helps/";
			}	
			else if (lcname.startsWith("file:") || lcname.startsWith("文件:"))
			{
				filename=filename.substring(filename.indexOf(':') + 1);
				dir=outputDir+"files/";
			}	
			else
			{
				dir=outputDir+"pages/";
			}
			if (buggy)
				dir +="buggy/";
		}
		else
		{
				dir=climbDirs;
		}
		if(subFolder.length() != 0){
			dir += subFolder + "/";
		}	
		filename = cleanInternalLink(filename);
		//System.out.println("new filename: "+filename);
		
		String new_filename="";

		String dirfilename=filename;
		
		new_filename = dir;
		dir="";
		
		if (useSuffixes)
			dirfilename=filenamepatch+dirfilename;
		else
			dirfilename+=filenamepatch;

		int pos;
		
		if (useSuffixes) pos=dirfilename.length()-1;
		else pos=0;
		
		for (int i=0;i<numDirLevels;i++)
		{
			if (useSuffixes)
			{
				String tmp="";
				for (int j=0;j<levelSize;j++)
				{
					String add=dirfilename.substring(pos, pos+1);
					pos--;
					if ((pos>=0)&&(dirfilename.charAt(pos)=='$'))
					{
						add+="$";
						pos--;
					}
					tmp=add+tmp;
				}
				dir=tmp+"/"+dir;
			}
			else
			{
				for (int j=0;j<levelSize;j++)
				{
					dir+=dirfilename.substring(pos, pos+1);
					pos++;
					if ((pos<dirfilename.length())&&(dirfilename.charAt(pos)=='$'))
					{
						dir+="$";
						pos++;
					}
				}
				dir+="/";					
			}
		}

		new_filename = new_filename + dir + filename+".xml";
		
		if (new_filename.length()>255)
		{
			System.out.println("filename '"+new_filename+"' is too long, truncate it");
			new_filename = new_filename.substring(0,new_filename.length()-4).substring(0,251)+".xml";
		}

		if ((create)&&(!relative))
			checkAllDirs(new_filename);

		return new_filename;
	}

	private boolean dumpXMLFile(
			String filename,
			String title,
			String content, boolean buggy)
	{
		//System.out.println("dumpXMLFile("+filename+","+content.length()+" bytes)");
		
		try {

			filename = createFileName(filename, title, false,null,buggy,true);

			File outputFile = new File(filename);
			FileOutputStream stream=new FileOutputStream(outputFile);
			
			Writer out = new BufferedWriter(new OutputStreamWriter(stream,"UTF-8"));

			//new ByteArrayInputStream(content.getBytes());

			out.write(content);
			out.flush();
			out.close();
			
			stream.close();
			
			generatedCount++;
		}
		catch (Exception e)
		{
			System.out.println(filename+":");
			e.printStackTrace();
			abortedCount++;
			generatedCount--;
			return false;
		}
		
		return true;
	}
	
	private void checkDir(String dirName) {
//		System.out.println("checkDir("+dirName+")");
		
		File dir = null;
		dir= new File(dirName);
		if (!dir.exists()) {
			try {
				dir.mkdir();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	private void checkAllDirs(String filename)
	{
	//	System.out.println("checkAllDirs("+filename+")");
		int i=0;
		int j;
		while ((j=filename.substring(i).indexOf("/"))!=-1)
		{
			checkDir(filename.substring(0,i+j));
			i=i+j+1;
		}
	}

	private boolean testXML(String file)
	{
		if (createTags==false) return true;
		
		boolean result=true;
		return result;
//		try
//		{
//			StringReader reader=new StringReader(file);
//			
//			InputSource input = new InputSource(reader);
//			input.setSystemId(createFileName(useArticleIDs?currentID:currentTitle,false,null,false,false));
//			
//			parser.parse(input,handler);
//			
//			reader.close();
//		}
//		catch(SAXParseException e)
//		{
//			e.printStackTrace();
//			System.out.println("SAXParse exception while testing XML: "+e.getMessage());				
//			result=false;
//		}
//		catch(Exception e)
//		{
//			e.printStackTrace();
//			problem("exception: "+e);
//			result=false;
//		}
//		
//		return result;
	}
	
	private void status()
	{
		if (phase==0) statusPhase0();
		else 		  statusPhase1();
	}
	
	private void statusPhase0()
	{
		if (articleCount%10000==0)
			System.out.println("\n[0] read "+articleCount+", "+articles.size()+" articles, "+redirections.size()+" redirections, "+abortedCount+" ignored\n");
	}

	private void statusPhase1()
	{
		System.out.println("\n[1] read "+articleCount+", generated "+generatedCount+", aborted "+abortedCount+", invalid "+invalidCount);
		System.out.println("[1] "+numTemplateInvocations+" template invocations, "+numBuiltinTemplates+" builtin templates, "+numTemplatesWithParameters+" with parameters\n");
	}

	// remove all tags from the text
	private String removeTags(String text)
	{
		// tag removal is done as preprocessing step
		
		if (true) return text;
		
		if (text==null) return "";
		
		char data[]=text.toCharArray();
		char newdata[]=new char[data.length];
		
		int read=0;
		int newptr=0;
		
		for (int i=0;i<data.length;i++)
		{
			if (data[i]=='<')
			{
				read++;
				continue;
			}
			else if (data[i]=='>')
			{
				read--;
				continue;
			}
			else if (read==0)
			{
				newdata[newptr++]=data[i];
			}
		}
		
		return new String(newdata,0,newptr);
	}
	
	private String removeComments(String text)
	{
//        System.out.println("content="+text+"\n**************************");
        
		StringBuffer result=new StringBuffer();
		
		// have a tag open
		boolean inTag=false;

		// saw LT in the previous step
		boolean sawLT=false;
		
		// in comment
		boolean inComment=false;
		
		// saw end of comment in last step (must be at the end of the token)
		boolean sawCommentEnd=false;
		
        // nesting level of nowiki
        int nowiki=0;
        
        StringTokenizer tokenizer = new StringTokenizer(text,"<>", true);

        while (tokenizer.hasMoreTokens())
        {
        	String token=tokenizer.nextToken();
        	
        	if (token.equals("<"))
        	{
//        		System.out.println("<: inTag="+inTag+", inComment="+inComment+", sawCommentEnd="+sawCommentEnd);
        		if (inTag==false) 
        		{
        			inTag=true;
        			sawLT=true;
        		}
        		else if (inComment==false)
        			result.append("<");
        	}
        	
        	else if (token.equals(">"))
        	{
//        		System.out.println(">: inTag="+inTag+", inComment="+inComment+", sawCommentEnd="+sawCommentEnd);
    			if (inTag==true)
        		{
        			inTag=false;
            		if (inComment==false) result.append(" "); // to make sure that markups that were aparted by the tag before
					// are not glued together
        		}
        		else
        		{
        			if (inComment==false) result.append(">");
        		}

    			if ((inComment)&&(sawCommentEnd))
    			{
    				sawCommentEnd=false;
    				inComment=false;
//    				System.out.println("end of comment");
    			}
        	}
        	
        	else if ((sawLT)&&(token.startsWith("!--")))
        	{
//        		System.out.println("!--: inTag="+inTag+", inComment="+inComment+", sawCommentEnd="+sawCommentEnd+" sawLT="+sawLT);
        		inComment=true;
//        		System.out.println("begin comment, skip "+token);
        		if (token.endsWith("--")) sawCommentEnd=true;
        		else sawCommentEnd=false;
        		sawLT=false;
        		continue;
        	}        	
        	else if ((inComment)&&(token.endsWith("--")))
        	{
        		sawCommentEnd=true;
        		sawLT=false;
        		continue;
        	}
        	else
        	{
//        		System.out.println("** else case, inTag="+inTag+", inComment="+inComment+", token="+token);
        		
        		if (inComment==false)
        		{
	        		if ((inTag)&&(token.equals("nowiki")))
	        		{
                        result.append("<nowiki>");
                        nowiki++;
                    }
	        		else if ((inTag)&&(token.equals("/nowiki")))
                    {
                        result.append("</nowiki>");
                        nowiki--;
                    }
	        		else if ((inTag)&&(token.equals("pre")))
                    {
                        result.append("<nowiki>");
                        nowiki++;
                    }
	        		else if ((inTag)&&(token.equals("/pre")))
                    {
                        result.append("</nowiki>");
                        nowiki--;
                    }
	        		else if ((inTag)&&(token.equals("code")))
                    {
                        result.append("<nowiki>");
                        nowiki++;
                    }
	        		else if ((inTag)&&(token.equals("/code")))
                    {
                        result.append("</nowiki>");
                        nowiki--;
                    }
                    else if ((inTag)&&(token.equals("math")))
                    {
                        result.append("<math>");
                        nowiki++;
                    }
                    else if ((inTag)&&(token.equals("/math")))
                    {
                        result.append("</math>");
                        nowiki--;
                    }
	        		else if (inTag==false)
	        		{
//	        			System.out.println("append it");
	        			result.append(token);
	        		}
	        		else if ((inTag)&&(Character.isLetter(token.charAt(0))==false)&&(token.startsWith("!--")==false)&&(token.startsWith("/")==false)&&(sawLT))
	        		{
	        			// looked like a tag, but this was wrong, as the following character is not legal for XML or a comment
	        			inTag=false;
	        			result.append(token);
	        		}
                    else if ((inTag)&&(nowiki>0))
                    {
//                      System.out.println("append it");
                        result.append(token);
                    }                        
        		}
        		else if (sawCommentEnd)
        		{
        			inComment=false; sawCommentEnd=false; inTag=false;
//        			System.out.println("end of comment");
        		}
        		else if (inComment)
        		{
        			if (token.endsWith("--")) sawCommentEnd=true;
        		}
        		else 
        		{
//        			System.out.println("skip "+token);
        		}
        		
        		sawLT=false;
        		sawCommentEnd=false;
        		
        	}
        }
		return result.toString();
	}

	private String removeOnlyComments(String text)
	{
//        System.out.println("content="+text+"\n**************************");
        
		StringBuffer result=new StringBuffer();
		
		int idx=0;
		
		while (idx!=-1)
		{
			int start=text.indexOf("<!-",idx);
			
			if (start==-1) break;
			
			int end=text.indexOf("->", start);
			if (end==-1) break;
			
			result.append(text.substring(idx, start));
			idx=end+2;
		}
		
		result.append(text.substring(idx));
		
		return result.toString();
	}

	private String cssLink()
	{
		StringBuffer res=new StringBuffer("<?xml-stylesheet type=\"text/css\" href=\"../../");
		
		// depending on the title of the current article, we have to climb to the root directory
		
		int num=0;
		char chars[]=currentTitle.toCharArray();
		for (int i=0;i<chars.length;i++)
		{
			if (chars[i]=='/') num++;
		}

		// add the corresponding number of "../" steps to move to the base article's root directory
		for (int i=0;i<num;i++)
			res.append("../");

		res.append("wikipedia.css\"?>\n");
		
		return res.toString();
	}

	private String xsltLink()
	{
		StringBuffer res=new StringBuffer("<?xml-stylesheet type=\"application/xml\" href=\"../../");
		
		// depending on the title of the current article, we have to climb to the root directory
		
		int num=0;
		char chars[]=currentTitle.toCharArray();
		for (int i=0;i<chars.length;i++)
		{
			if (chars[i]=='/') num++;
		}

		// add the corresponding number of "../" steps to move to the base article's root directory
		for (int i=0;i<num;i++)
			res.append("../");

		res.append("wikipedia.xslt\"?>\n");
		
		return res.toString();
	}

	private String dtdLink()
	{
		StringBuffer res=new StringBuffer("<!DOCTYPE ");

		if (isRedirect)
			res.append("redirection");
		else
			res.append("article");
		
		res.append(" SYSTEM \"");
			res.append(climbDirs);

			// for INEX09, all articles are at the same level, no more changes needed here.
			
		res.append("article.dtd\">\n");
		
		return res.toString();
	}
	
	private void testRedirection(String content)
	{
		String title=currentTitle;
		
		if (title.length()>1)
		{
			title=Character.toUpperCase(title.charAt(0))+title.substring(1);
		}
		else if (title.length()==1)
			title=Character.toString(Character.toUpperCase(title.charAt(0)));
		
		if ((content.length()>"#redirect".length())&&(content.substring(0,"#redirect".length()).toLowerCase().startsWith("#redirect")))
		{
			isRedirect=true;
			
	        StringTokenizer tokenizer = new StringTokenizer(content,"='[]{}|*\n!+-#:", true);
	        while (tokenizer.hasMoreTokens())
	        {
	        	String token=tokenizer.nextToken();
	        	if (token.equals("["))
	        	{
	        		if (tokenizer.hasMoreTokens())
	        		{
	        			token=tokenizer.nextToken();
	    	        	if (token.equals("["))
	    	        	{
	    	        		if (tokenizer.hasMoreTokens())
	    	        		{
	    	        			token=tokenizer.nextToken();
	    		        		// we have found the start for the link
	    		        		String target=findRedirectionTarget(token,tokenizer);
	    		        		redirections.put(title,target);
	    		        		//System.out.println("redirect ["+currentTitle+"] to ["+target+"]");
	    	        		}
	    	        	}
	        		}
	        	}
	        }
		}
	}
	
	private String findRedirectionTarget(String token, StringTokenizer tokenizer)
	{	
		String result="";

		boolean validLink=true;
		
		// peek forward
		
		while (true)
		{
			String next=null;
			if (tokenizer.hasMoreTokens()) next=tokenizer.nextToken();
			else break; // looks like the document is broken
			
			if (next.compareTo("]")==0)
			{
				//the link is finished here
				
				if (token.indexOf(':')!=-1)
				{
					if (token.indexOf(":")==0)
					{
						// this link links to the description of the (category,image,...)
						// this is always in the output* directories
						token=token.substring(1);
					}
					else if (token.startsWith("Help:"))
						validLink=true;
					else
					{
						// unexpected type of link, so ignore it
						validLink=false;				
					}
				}		

				// the token has to be converted to an URL (we'll do this later)

				if (token.indexOf('|')!=-1)
				{
					token=token.substring(0,token.indexOf('|'));
				}
				
				// quick fix for invalid documents
				if (tokenizer.hasMoreTokens()) tokenizer.nextToken();
				if (validLink)
				{
					result=token;
				}
				
				break;
			}
			else 
			{
				token+=next;
				//caption=next;
			}
		}

		if (result.length()>1)
		{
			result=Character.toUpperCase(result.charAt(0))+result.substring(1);
		}
		else if (result.length()==1)
			result=Character.toString(Character.toUpperCase(result.charAt(0)));

		return result;
	}

	public void reset(int p)
	{
		phase=p;
		
		articleCount=0;
		abortedCount=0;
		generatedCount=0;
		invalidCount=0;
	}
	
	private String handleTemplates(String content)
	{
		templates=new HashMap<String,String>();
		templateNames=new HashMap<String,String>();
		lasttemplateid=0;
		
		StringBuffer result=new StringBuffer();
		
		int pos=0;
		int start=0;
		int end=0;
		
		boolean didSomething=false;
		
		while (true)
		{
			pos=0;
			didSomething=false;
			result=new StringBuffer(content.length());
			
			while ((pos!=-1)&&(pos<content.length()))
			{

				// fast forward to the first occurrence of a template call
				
				start=content.indexOf("{{",pos);
				
				if (start!=-1)
				{
					end=content.indexOf("}}",start+2);
					if (end==-1) // buggy template invocation
					{
						pos=-1;
						continue;
					}
					int s2=content.indexOf("{{",start+2);
					
					if ((s2!=-1)&&(s2<end))
					{
						// there is another template invocation earlier, so handle this first
						result.append(content.substring(pos,start+1));
						pos=start+1;
						continue;
					}

					didSomething=true;
					result.append(content.substring(pos,start));
				
					String templateText=content.substring(start+2,end);
			
					// check if the template call is surrounded by <nowiki> tags; if so, ignore it
					// (and silently remove the template call from the output}
					
					boolean ignoreme=false;
					
					int nowikistart=content.substring(0,start).lastIndexOf("<nowiki>");
					if (nowikistart!=-1)
					{
						int nowikiend=content.substring(0,start).lastIndexOf("</nowiki>");
						if ((nowikiend==-1)||(nowikiend<nowikistart))
						{
							nowikiend=content.substring(end+2).indexOf("</nowiki>");
							if (nowikiend!=-1)
							{
								// ignore this template
								ignoreme=true;
							}
						}
					}
			
					if (ignoreme)
					{
						if (debugTemplates) System.out.println("ignore template invocation: "+templateText);
					}
					else
					{
//						if (debugTemplates) System.out.println("template invocation: "+templateText);
						
						// expand the template using its definition, replacing the parameters
						
//						if (templateText.startsWith("Infobox Officeholder"))
//							System.out.println("IB OH");
						
						String r=expandTemplate(templateText);
						
//						String r=handleTemplate(templateText);
//						r=replacePipes(r);
						
						result.append(r);
						
//						System.out.println("BEFORE: "+content.substring(pos,start+1));
//						System.out.println("AFTER:  "+content.substring(end+2));
					}
//					if (content.substring(end+2).startsWith("}}"))
//						System.out.println("potential problem.");
					
					result.append(content.substring(end+2));
					content=result.toString();
					
//					int cnt=0;
//					for (int x=0;x<content.length();x++)
//					{
//						if (content.charAt(x)=='{') cnt++;
//						else if (content.charAt(x)=='}') cnt--;
//					}
//					if (cnt!=0)
//					{
//						System.out.println("unbalanced number of braces!");
//					}
					result=new StringBuffer(content.length());
					pos=0;
					continue;
				}
				else break;
			}
			if (pos!=-1) result.append(content.substring(pos));
			pos=0;
			
			if (didSomething==true)
			{
//				if (result.length()==0)
//					System.out.println("oops");
				content=result.toString();
//				System.out.println("****\n"+content+"***\n");
				continue;
			}
			else
				return getPipesBack(content);
		}
	}

	int numTemplateInvocations=0;
	int numBuiltinTemplates=0;
	int numTemplatesWithParameters=0;
	
	final static HashSet<String> internalTemplates = new HashSet<String>();
	static
	{
		internalTemplates.add("PAGENAME");
		internalTemplates.add("NAMESPACE");
		internalTemplates.add("NAMESPACEE");
		internalTemplates.add("TALKSPACE");
		internalTemplates.add("FULLPAGENAME");
		internalTemplates.add("TALKPAGENAME");
		internalTemplates.add("FULLPAGENAMEE");
		internalTemplates.add("SERVER");
		internalTemplates.add("CURRENTDAY");
		internalTemplates.add("CURRENTDOW");
		internalTemplates.add("CURRENTYEAR");
		internalTemplates.add("CURRENTDAY2");
		internalTemplates.add("CURRENTYEAR");
		internalTemplates.add("CURRENTMONTHABBREV");
		internalTemplates.add("CURRENTMONTHNAME");
		internalTemplates.add("CURRENTTIME");
		internalTemplates.add("CURRENTTIMESTAMP");
		internalTemplates.add("CURRENTHOUR");
		internalTemplates.add("CURRENTWEEK");
		internalTemplates.add("#if:");
		internalTemplates.add("#ifexpr:");
		internalTemplates.add("#expr:");
		internalTemplates.add("#ifeq:");
		internalTemplates.add("#switch:");
		internalTemplates.add("#pad:");
		internalTemplates.add("padleft:");
		internalTemplates.add("padright:");
		internalTemplates.add("lc:");
		internalTemplates.add("uc:");
		internalTemplates.add("ns:");
		internalTemplates.add("lcfirst:");
		internalTemplates.add("ucfirst:");
		internalTemplates.add("#time:");
		internalTemplates.add("raw:");
		internalTemplates.add("fullurl:");
		internalTemplates.add("CURRENTMONTH");
		internalTemplates.add("CURRENTTIME");
		internalTemplates.add("CURRENTTIME");
		internalTemplates.add("CURRENTTIME");
	}
	private String handleTemplate(String templateText)
	{
//		if (debugTemplates) System.out.println("handleTemplate("+templateText+")");
		
		// count template invocations
		
		numTemplateInvocations++;
//		numBuiltinTemplates++;
		

		String t1="--NT--";
		if (templateText.indexOf(':')!=-1)
			t1=templateText.substring(0,templateText.indexOf(':')+1);
		
//		System.out.println("probe with "+t1+" and "+templateText);
		
		boolean haveParserFunction=(internalTemplates.contains(templateText)||(internalTemplates.contains(t1)));
//		if ((templateText.startsWith("#if:"))
//				|| (templateText.startsWith("#switch:"))
//				|| (templateText.startsWith("#ifexist:"))
//				|| (templateText.startsWith("#ifeq:"))
//				|| (templateText.startsWith("#ifexpr:"))
//				|| (templateText.startsWith("lcfirst:"))
//				|| (templateText.startsWith("ucfirst:"))
//				|| (templateText.startsWith("#pad"))
//				|| (templateText.startsWith("padleft:"))
//				|| (templateText.startsWith("padright:"))
//				|| (templateText.startsWith("lc:"))
//				|| (templateText.startsWith("#expr:"))
//				|| (templateText.startsWith("uc:"))
//				|| (templateText.startsWith("ns:"))
//				|| (templateText.startsWith("raw:"))
//				|| (templateText.startsWith("fullurl:"))
//				|| (templateText.startsWith("#time:")))
//			haveParserFunction=true;
		
		if (haveParserFunction) numBuiltinTemplates++;
		
		String templateName;
		int namepos=0;
		
		if (haveParserFunction) namepos=templateText.indexOf(":");
		else namepos=templateText.indexOf("|");
		
		if (namepos==-1)
		{
			// no parameters
			templateName=templateText.trim();
		}
		else
		{
			templateName=templateText.substring(0,namepos).trim();
			templateText=templateText.substring(namepos+1);
		}
		
		// do something with the template's parameters
		
		HashMap<String,String> parameters=new HashMap<String,String>();
		HashMap<String,String> namedParameters=new HashMap<String,String>();
		
		String parameter="";
		String value="";
		int numParam=0;
		
		boolean isParam=true;
		int numSquareBrackets=0;
		
		for (int pos=0;pos<templateText.length();pos++)
		{
			char c=templateText.charAt(pos);
			
			if (c=='[')
			{
				numSquareBrackets++;
				if (isParam) parameter+=c;
				else 		 value+=c;
			}
			else if (c==']')
			{
				numSquareBrackets--;
				if (isParam) parameter+=c;
				else 		 value+=c;
			}
			
			else if (c=='|')
			{
				if (numSquareBrackets>0)
				{
					if (isParam==false) value+="|";
				}
				else
				{
					// parameter separator
					
					if ((parameter.length()>0)&&(value.length()==0))
					{
						value=parameter;
						parameter=null;
					}
					else if (parameter.length()==0)
					{
						parameter=null; // can this happen at all?
					}
					
//					numParam++;
					
					if (parameter!=null) parameters.put(parameter.trim(),value.trim());
					else if (value.length()>0)
					{
						numParam++;
						parameters.put(String.valueOf(numParam),value.trim());
					}

					if (parameter!=null) namedParameters.put(parameter.trim(),value.trim());
					else if (value.length()>0) namedParameters.put(String.valueOf(numParam),value.trim());

					parameter="";
					value="";
					isParam=true;
				}
			}
			else if (c=='=')
			{
				// parameter-value-separator
				isParam=false;
			}
			else
			{
				if (isParam) parameter+=c;
				else 		 value+=c;
			}
		}
		
		// handle the last parameter
		
		if ((parameter.length()>0)&&(value.length()==0))
		{
			value=parameter;
			parameter=null;
		}
		else if (parameter.length()==0)
		{
			parameter=null; // can this happen at all?
		}
		
//		numParam++;
		
		if (parameter!=null) parameters.put(parameter.trim(),value.trim());
		else if (value.length()>0)
		{
			numParam++;
			parameters.put(String.valueOf(numParam),value.trim());
		}

		if (parameter!=null) namedParameters.put(parameter.trim(),value.trim());
		else if (value.length()>0) namedParameters.put(String.valueOf(numParam),value.trim());

		if (parameters.isEmpty()==false) numTemplatesWithParameters++;
		
		System.out.println("handleTemplate("+templateName+") ("+haveParserFunction+")");
		System.out.println("parms: "+parameters);
//		Iterator<Map.Entry<String,String>> it=parameters.entrySet().iterator();
//		while (it.hasNext())
//		{
//			Map.Entry<String,String> e=it.next();
//			System.out.println(e.getKey()+":"+e.getValue());
//		}
		
		// do something with the template

		StringBuffer result=new StringBuffer();
		
		if (haveParserFunction)
		{
			// do simple things first

			int i=currentTitle.indexOf(":");
			
			if (templateText.compareTo("PAGENAME")==0)
			{
				if (i!=-1) return XML.XMLify(currentTitle.substring(i+1));
				else return XML.XMLify(currentTitle);
			}
			if (templateText.compareTo("NAMESPACE")==0)
			{
				if (i!=-1) return XML.XMLify(currentTitle.substring(0,i));
				else return "";
			}
			if (templateText.compareTo("NAMESPACEE")==0)
			{
				if (i!=-1) return XML.XMLify(currentTitle.substring(0,i));
				else return "";
			}
			if (templateText.compareTo("TALKSPACE")==0)
			{
				return "";
			}
			if (templateText.compareTo("TALKPAGENAME")==0) return "";
			
			if (templateText.compareTo("FULLPAGENAME")==0)
				return XML.XMLify(currentTitle);
			if (templateText.compareTo("FULLPAGENAMEE")==0)
				return XML.XMLify(currentTitle);

			
			if (templateText.compareTo("SERVER")==0)
				return "www.mpi-inf.mpg.de";

			// this is currently static. I don't see why it should be made dynamic.
			
			if (templateText.compareTo("CURRENTYEAR")==0)
				return "2008";

			if (templateText.compareTo("CURRENTDAY")==0)
			{
				return "20";
			}
			if (templateText.compareTo("CURRENTDAYNAME")==0)
			{
				return "Sunday";
			}
			if (templateText.compareTo("CURRENTDOW")==0)
			{
				return "2";
			}
			if (templateText.compareTo("CURRENTDAY2")==0)
			{
				return "20";
			}

			if (templateText.compareTo("CURRENTMONTH")==0)
			{
				return "12";
			}
			if (templateText.compareTo("CURRENTMONTHABBREV")==0)
			{
				return "Dec";
			}
			if (templateText.compareTo("CURRENTMONTHNAME")==0)
			{
				return "December";
			}
			if (templateText.compareTo("CURRENTTIME")==0)
			{
				return "18:00";
			}
			if (templateText.compareTo("CURRENTHOUR")==0)
			{
				return "18";
			}
			if (templateText.compareTo("CURRENTWEEK")==0)
			{
				return "51";
			}
			if (templateText.compareTo("CURRENTTIMESTAMP")==0)
			{
				return "20081220180000";
			}
			if (templateName.compareTo("#if")==0)
			{
				if (parameters.containsKey("1"))
				{
					if (parameters.containsKey("2")) return parameters.get("2");
				}
				else if (parameters.containsKey("3")) return parameters.get("3");
				else return "";
			}
			else if (templateName.compareTo("#ifexpr")==0)
			{
				System.out.println("[DEBUG] unsupported call to #ifexpr, using defaults.");
				// we do not support evaluating expressions at this time, hence we always return the second parameter
				if (parameters.containsKey("1"))
				{
					if (parameters.containsKey("2")) return parameters.get("2");
				}
//				else if (parameters.containsKey("3")) return parameters.get("3");
				else return "";
			}
			else if (templateName.compareTo("#ifexist")==0)
			{
				System.out.println("[DEBUG] unsupported call to #ifexist, using defaults.");
				// we do not support this at this time
				String r2=parameters.get("2");
				if (r2!=null) return r2;
				else return "";
			}
			else if (templateName.compareTo("#ifeq")==0)
			{
				String r1=parameters.get("1");
				String r2=parameters.get("2");
				if ((r1!=null)&&(r2!=null))
				{
					if (r1.equals(r2))
					{
						String r3=parameters.get("3");
						if (r3!=null) return r3;
						else return "";
					}
					else
					{
						String r4=parameters.get("4");
						if (r4!=null) return r4;
						else return "";
					}
				}
//				else if (parameters.containsKey("3")) return parameters.get("3");
				else return "";
			}
			else if (templateName.compareTo("lcfirst")==0)
			{
				String p1=parameters.get("1");
				if (p1!=null)
				{
					if (p1.length()==0) return "";
					else if (p1.length()==1) return p1.toLowerCase();
					else return p1.substring(0,1).toLowerCase()+p1.substring(1);
				}
				else return "";
			}
			else if (templateName.compareTo("ucfirst")==0)
			{
				String p1=parameters.get("1");
				if (p1!=null)
				{
					if (p1.length()==0) return "";
					else if (p1.length()==1) return p1.toUpperCase();
					else return p1.substring(0,1).toUpperCase()+p1.substring(1);
				}
				else return "";
			}
			else if (templateName.compareTo("lc")==0)
			{
				String p1=parameters.get("1");
				if (p1!=null)
				{
					return p1.toLowerCase();
				}
				else return "";
			}
			else if (templateName.compareTo("uc")==0)
			{
				String p1=parameters.get("1");
				if (p1!=null)
				{
					return p1.toUpperCase();
				}
				else return "";
			}
			else if (templateName.compareTo("#time")==0)
			{
				System.out.println("unsuported call to #time, returning default.");
				return "00:00";
			}			
			else if (templateName.compareTo("#expr")==0)
			{
				System.out.println("unsuported call to #expr, returning default.");
				String p1=parameters.get("1");
				if (p1!=null)
				{
					return p1.toUpperCase();
				}
				else return "0";
			}			
			else if (templateName.compareTo("#switch")==0)
			{
				System.out.println("switch");
				
				String v=parameters.get("1");
				if (v==null) return ""; // we're dead.
				
				// do simple things first: if we have a parameter that corresponds to the switch variable, return this
				
				String r=parameters.get(v);
				if (r!=null) return r;
				
				System.out.println("unsupported SWITCH call, returning default.");
				return parameters.get(Integer.toString(parameters.size()));
			}
			else if (templateName.compareTo("ns")==0)
			{
				String p=parameters.get("1");
				if (p==null) return "";
				else if ((p.equals("-1"))||(p.equals("special"))) return "Special";
				else if ((p.equals("-2"))||(p.equals("media"))) return "Media";
				else if ((p.equals("1"))||(p.equals("talk"))) return "Talk";
				else if ((p.equals("2"))||(p.equals("user"))) return "User";
				else if ((p.equals("3"))||(p.equals("user_talk"))) return "User talk";
				else if ((p.equals("4"))||(p.equals("project"))) return "Wikipedia";
				else if ((p.equals("5"))||(p.equals("project_talk"))) return "Wikipedia talk";
				else if ((p.equals("6"))||(p.equals("image"))) return "Image";
				else if ((p.equals("7"))||(p.equals("image_talk"))) return "Image talk";
				else if ((p.equals("8"))||(p.equals("mediawiki"))) return "Mediawiki";
				else if ((p.equals("9"))||(p.equals("mediawiki_talk"))) return "Mediawiki talk";
				else if ((p.equals("10"))||(p.equals("template"))) return "Template";
				else if ((p.equals("11"))||(p.equals("template_talk"))) return "Template talk";
				else if ((p.equals("12"))||(p.equals("help"))) return "Help";
				else if ((p.equals("13"))||(p.equals("help_talk"))) return "Help talk";
				else if ((p.equals("14"))||(p.equals("category"))) return "Category";
				else if ((p.equals("15"))||(p.equals("category_talk"))) return "Category talk";
			}
			else if (templateName.compareTo("fullurl")==0)
			{
				String p=parameters.get("1");
				if (p!=null)
				{
					return makeWikiURL(p);
				}
				else return "";
			}
			else if (templateName.compareTo("padleft")==0)
			{
				String p=parameters.get("1");
				if (p!=null)
				{
					return p;
				}
				else return "";
			}
			else if (templateName.compareTo("padright")==0)
			{
				String p=parameters.get("1");
				if (p!=null)
				{
					return p;
				}
				else return "";
			}
			else if (templateName.compareTo("#pad")==0)
			{
				String p=parameters.get("1");
				if (p!=null)
				{
					return p;
				}
				else return "";
			}
			else
			{
				problem("unexpected internal template: "+templateName);
				return "";
			}
		}
		else
		{
			if (templateName.compareTo("Lifetime")==0)
				System.out.println("here.");
			
			String template=loadTemplate(templateName);
		
			// quick workaround: we do not support if/else statements within templates
			
			if (template.indexOf("{{if")!=-1)
			{
				if (debugTemplates) System.out.println("\ttemplate contains if, ignore it.");
				return "";
			}
			
			template=removeComments(template);
			
			if (template.startsWith("{{{{"))
				System.out.println("???");
			
			template=replaceParameters(template,parameters);
			
//			System.out.println("RESULT: "+template);
			
			if (true) return template;
			
			int pos=0; int start=0; int end=0;
			
			while (pos!=-1)
			{
				start=template.substring(pos).indexOf("{{{");
				if (start!=-1)
				{
					end=template.substring(pos+start).indexOf("}}}");
					if (end==-1)
					{
						// buggy
						pos=-1;
						continue;
					}
					
					int s2=template.substring(pos+start+3).indexOf("{{{")+3;
					if ((s2!=2)&&(s2<end))
					{
						// something's wrong with this template (at least we don't support this)
						System.out.println("unsupported nesting of parameters [s2="+(s2)+", end="+end+"], abort.");
						//empty the result to get rid of any partial results
						result=new StringBuffer();
						break;
					}
					if (start>0) result.append(template.substring(pos,pos+start));
					
					String param=template.substring(pos+start+3,pos+start+end);
					
					if (debugTemplates)
						System.out.println("\t param: "+param);
					
					// replace
					
					String defValue="";
					
					int def=param.indexOf("|");
					if (def!=-1)
					{
						defValue=param.substring(def+1).trim();
						param=param.substring(0,def).trim();
					}
					
					//System.out.println("parameter call: param="+param+", default="+defValue+", "+parameters.containsKey(param));
		
					if (parameters.containsKey(param))
						result.append(parameters.get(param));
					else result.append(defValue);
					
					pos=pos+start+end+3;
				}
				else
				{
					result.append(template.substring(pos));
					pos=-1;
				}
			}
			}
		System.out.println(result);
		
		
		return result.toString();
	}
	
	// replace all parameters in the template invocation with their actual values (or the defaults)
	// note that this does not replace any embedded template calls
	
	private String replaceParameters(String template, HashMap<String,String> parameters)
	{
//		if (template.startsWith("{|"))
//			System.out.println("replaceParameters("+template+","+parameters+")");
		
		int pos=0;
		int start=0;
		
		boolean didSomething=false;
		
		while (true)
		{
			while ((pos!=-1)&&(pos<template.length()))
			{
				start=template.indexOf("{{{",pos);
				
				if (start==0)
					System.out.print("####");
				
				if (start==117)
					System.out.println("stop");
				
				// this should be the start of a parameter occurrence (if it's not, we're dead)
				
				if (start==-1) break; // no more parameter invocations, done!
				
				int end=template.indexOf("}}}",start+2);
				
				// try simple things first: if there are no further opening braces in the interval start-end,
				// we're done identifying a parameter invocation;
				// otherwise we need to find the correct closing position
				// (or we may even find another, embedded parameter invocation first)		
				
				int p=template.indexOf('{',start+1);
				if ((p!=-1)&&(p<end))
				{
					int s2=template.indexOf("{{{",start+1);
					if ((s2!=-1)&&(s2<end))
					{
						// yet another parameter invocation found within the scope,
						// consider this first
						pos=s2;
						continue;
					}
					// scan until we find }}}, taking any opening braces we find into account
					int opens=0;int i;
//					System.out.println("start loop: "+template.substring(start+3));
					
					for (i=start+3;i<template.length();i++)
					{
						if (template.charAt(i)=='{') opens++;
						else if (template.charAt(i)=='}') opens--;
						if (opens==-3) break;
					}
					if (opens!=-3)
					{
						problem("confused: expected at least 3 closing brackets (opens="+opens+")");
					}
					end=i-2;
				}
				
				// here we go: the parameter invocation lasts from start+2 to end
				
				if (end<0)
					System.out.println("bug!");
				
				String param=template.substring(start+3,end);
				String deflt="";
				int def=template.indexOf('|',start+2);
				if ((def!=-1)&&(def<end))
				{
					deflt=param.substring(def-start-2);
					param=template.substring(start+3,def);
				}
				String res=template.substring(0,start);
				if (parameters.containsKey(param))
					res+=parameters.get(param);
				else
					res+=deflt;
				res+=template.substring(end+3);
				didSomething=true;
				
				int cnt=0;
				for (int x=0;x<res.length();x++)
				{
					if (res.charAt(x)=='{') cnt++;
					else if (res.charAt(x)=='}') cnt--;
				}
				if (cnt!=0)
				{
					System.out.println("unbalanced number of braces!");
				}

				template=res;

				break;
			}
//			System.out.println("loop done (pos="+pos+") : "+template.length());
			if (didSomething)
			{
				didSomething=false;
				pos=0;
			}
			else
				break;
		}
		return template;
	}
	
	private String loadTemplate(String template)
	{
		if (debugTemplates) System.out.println("loadTemplate("+template+")");
		
		String filename=createTemplateFileName(template);
		
//		if (debugTemplates) System.out.println("filename: "+filename);
		
		File inputFile = new File(filename);
		FileInputStream stream=null;
		BufferedReader in=null;
		try
		{
			stream=new FileInputStream(inputFile);
			in = new BufferedReader(new InputStreamReader(stream,"UTF-8"));
		}
		catch(Exception e)
		{
			if (debugTemplates) System.out.println("loadTemplate("+template+"): "+e);
		}
		
		String res="";
		String read="";

		while (read!=null)
		{
			res+=read+"\n";
			try
			{
				read=in.readLine();
			}
			catch(Exception e)
			{
				if (debugTemplates) System.out.println("loadTemplate("+template+"), readLine: "+e);
				read=null;
			}
		}

		try
		{
			if (in!=null) in.close();
		}
		catch(Exception e)
		{}
		
		try
		{
			if (stream!=null) stream.close();
		}
		catch(Exception e)
		{}

		res=res.trim();
		
		//System.out.println("(before) res="+res);
		
		if (res.toLowerCase().startsWith("#redirect"))
		{
			String redirect=res.substring(res.indexOf("[[")+2);
			if (redirect.toLowerCase().startsWith("template:")==false)
			{
				if (debugTemplates) System.out.println("\ttemplate redirect to content page "+redirect+" unsupported, ignored");
				return "";
			}
			redirect=redirect.substring(redirect.indexOf(":")+1,redirect.indexOf("]]"));
			
			if (debugTemplates) System.out.println("\tredirect to "+redirect);
			
			return loadTemplate(redirect);
		}
		
		// remove <noinclude> text
		
		String text=res;
		res="";
		
		int pos=0;
		int start=0;
		int end=0;
		
		while (pos!=-1)
		{
			start=text.substring(pos).indexOf("<noinclude>");
			if (start!=-1)
			{
				end=text.substring(pos+start).indexOf("</noinclude>");
				if (end==-1)
				{
					// buggy
					pos=-1;
					continue;
				}
				
				if (start>0) res+=text.substring(pos,pos+start);
				
				pos=pos+start+end+12;
				if (pos>text.length()) pos=-1;
			}
			else
			{
				res+=text.substring(pos);
				pos=-1;
			}
		}
		
//		System.out.println("(after)  res="+res);

		return res;
	}
	
	private String createTemplateFileName(String filename)
	{
		
		if (filename.length()==0) return "";

		String lcname=filename.toLowerCase();
		if (lcname.startsWith("template:"))
			{
				filename=filename.substring(9);
			}
		
		filename=filename.replaceAll("\\s","_");
		
		if (filename.length()>0) filename = cleanInternalLink(filename);
		//System.out.println("new filename: "+filename);
		
		String new_filename="";

		new_filename = outputDir + "/"+ templateBaseURL+ "/" + getPrefix(filename) + "/" + filename+".wkt";
		
		if (new_filename.length()>255)
		{
			//System.out.println("filename '"+new_filename+"' is too long, truncate it");
			new_filename = new String(outputDir + "/" + templateBaseURL +"/"+ getPrefix(filename) + "/" + filename).substring(0,251)+".wkt";
		}

		new_filename=new_filename.replaceAll("//","/");
		
		checkAllDirs(new_filename);

		return new_filename;
	}

	private String createEntities(String content)
	{
		// replace all occurrences of '&' with its encoding
		
		content=content.replaceAll("&","&amp;");
		
		if (createEntities)
		{
			// check for known HTML entities and replace them back
			
			content=content.replaceAll("&amp;mdash;","&mdash;");
			content=content.replaceAll("&amp;ndash;","&ndash;");
			content=content.replaceAll("&amp;nbsp;","&nbsp;");
			content=content.replaceAll("&amp;sup2;","&sup2;");
			content=content.replaceAll("&amp;lsaquo;","&lsaquo;");
			content=content.replaceAll("&amp;rsaquo;","&rsaquo;");
			content=content.replaceAll("&amp;middot;","&middot;");
			content=content.replaceAll("&amp;#0","&#0");
			content=content.replaceAll("&amp;#1","&#1");
			content=content.replaceAll("&amp;#2","&#2");
			content=content.replaceAll("&amp;#3","&#3");
			content=content.replaceAll("&amp;#4","&#4");
			content=content.replaceAll("&amp;#5","&#5");
			content=content.replaceAll("&amp;#6","&#6");
			content=content.replaceAll("&amp;#7","&#7");
			content=content.replaceAll("&amp;#8","&#8");
			content=content.replaceAll("&amp;#9","&#9");
			content=content.replaceAll("&amp;quot;","\"");
			content=content.replaceAll("&amp;apos;","'");
		}
		
		return content;
	}
	
	private void openTag(StringBuffer result,String tag)
	{
		if (createTags) result.append("<").append(tag).append(">");
	}

	private void openTag(StringBuffer result, String tag, String attributes)
	{
		if (createTags)
		{
			if (attributes.length()>0)
			{
				attributes=fixAttributes(attributes);
			}
			
			result.append("<").append(tag).append(" ").append(XML.XMLify2(attributes)).append(">");			
		}

	}
	
	private String fixAttributes(String attributes)
	{
		HashMap<String,String> attrs=new HashMap<String,String>(); // stores the attributes after conversion
		
		boolean inName=true;
		boolean inValue=false;
		boolean hadSpace=false; // have seen spaces since last name started
		boolean hadQuote=false; // have seen exactly one quote
		boolean isValid=true; // attribute list is valid
		boolean missedQuote=false; // value without trailing quote
		
		boolean hadProblem=false;
		
		String currName="";
		String currValue="";
		
		char chars[]=attributes.toCharArray();
		int pos=0;
		
		for (;pos<chars.length;pos++)
		{
			char c=chars[pos];
			if (inName)
			{
				int nl=currName.length();
				if (Character.isWhitespace(c))
				{
					if (nl==0) continue; // ignore white space until name has started
					else
					{
						hadSpace=true;
						continue;
					}
				}
				else if (Character.isLetter(c))
				{
					if (hadSpace)
					{
						// space within names, we're dead.
						problem("attribute "+currName+" without value ignored");
						hadProblem=true;
						currName=Character.toString(c);
						hadSpace=false;
						continue;
					}
					else
					{
						// standard case: letters are always welcome in names
						currName+=c;
						continue;
					}
				}
				else if ((Character.isDigit(c))||(c==':')|(c==';')||(c=='-'))
				{
					if ((nl==0)||(hadSpace))
					{
						// no initial digits
						problem("ignore initial "+c+" in attribute name");
						hadProblem=true;
						continue;
					}
					else
					{
						// digits in later positions are ok
						currName+=c;
						continue;
					}
				}
				else if (c=='=')
				{
					// value begins
					if (nl==0)
					{
						problem("found value without name");
						hadProblem=true;
						break;
					}
					else
					{
						inValue=true;
						inName=false;
						continue;
					}
				}
				else
				{
					problem("unexpected character "+c+" in name, ignored.");
					hadProblem=true;
					continue;
				}
			}
			else if (inValue)
			{
				if (c=='\"')
				{
					if (hadQuote==false)
					{
						hadQuote=true;
						currValue+="\"";
						continue;
					}
					else
					{
						// done with this attribute
						currValue+="\"";
						attrs.put(currName,currValue);
						inName=true;
						hadSpace=false;
						inValue=false;
						currName="";
						currValue="";
						hadQuote=false;
						missedQuote=false;
						continue;
					}
				}
				else if (c=='\n') // remove any linebreaks from wherever in the attribute list they occur
					continue;
				else if (hadQuote==false)
				{
					if (Character.isWhitespace(c))
					{
						// ignore until we read the first nonspace character
						continue;
					}
					else
					{
						// missing quote
						currValue+="\""+c;
						hadQuote=true;
						missedQuote=true;
						continue;
					}
				}
				else
				{
					if ((missedQuote)&&(c==' '))
					{
						// done with this attribute
						currValue+='\"';
						attrs.put(currName,currValue);
						inName=true;
						hadSpace=false;
						inValue=false;
						currName="";
						currValue="";
						hadQuote=false;
						missedQuote=false;
						continue;
						
					}
					currValue+=c;
					continue;
				}
			}
		}
		
		if (pos==chars.length)
		{
			if (inValue)
			{
				if (currValue.length()>0)
				{
					currValue+="\"";
					attrs.put(currName,currValue);
				}
			}
		}
		else if (pos<chars.length)
		{
			isValid=false;
		}

		String result="";
		
		if (isValid)
		{
			Iterator<Map.Entry<String, String>> it=attrs.entrySet().iterator();
			while (it.hasNext())
			{
				Map.Entry<String,String> e=it.next();
				if (result.length()>0) result+=" ";
				result+=e.getKey()+"="+e.getValue();
			}
		}
		if (hadProblem)
			problem("original: "+attributes);
		return result;
	}

	private void openTagLn(StringBuffer result,String tag)
	{
		if (createTags)
		{
			openTag(result,tag);
		}
		result.append("\n");			
	}

    private void openTagLn(StringBuffer result, String tag, String attributes)
    {
    	if (createTags)
    	{
            openTag(result,tag,attributes);
    	}
        result.append("\n");    		
    }
	
    // special function to open tags in article headers with attributes that don't need to be XMLified
    private void openHeaderTagLn(StringBuffer result, String tag, String attributes)
    {
    	if (createTags)
    	{
    		result.append("<").append(tag).append(" ").append(attributes).append(">");
    	}
    	result.append("\n");
    }

    private void emptyHeaderTagLn(StringBuffer result, String tag, String attributes)
    {
    	if (createTags)
    	{
    		result.append("<").append(tag).append(" ").append(attributes).append("/>");
    	}
    }

    private void closeTag(StringBuffer result,String tag)
	{
		if (createTags)
		{
			String openingTag="<"+tag+">";
			int len=result.length();
			
			int sidx=result.lastIndexOf("<");
			int eidx=result.lastIndexOf(">");
			
			if ((sidx!=-1)&&(eidx!=-1)&&(sidx<eidx))
			{
				String lasttag=result.substring(sidx, eidx+1);
				String text=result.substring(eidx+1).trim();
				
	//			System.out.println(openingTag+"\t"+lasttag+"\t"+text.length());
				if ((text.length()==0)&&(lasttag.compareTo(openingTag)==0))
				{
	//				System.out.println("ignore empty tag sequence ("+tag+")");
					result.delete(sidx, len);
				}
				else result.append("</").append(tag).append(">");
			}
			else result.append("</").append(tag).append(">");
		}
	}

	private void closeTagLn(StringBuffer result,String tag)
	{
		if (createTags) closeTag(result,tag);
		result.append("\n");
	}

	private void emptyTag(StringBuffer result,String tag)
	{
		if (createTags) result.append("<").append(tag).append("/>");
	}

	private void emptyTagLn(StringBuffer result,String tag)
	{
		if (createTags) result.append("<").append(tag).append("/>");
		result.append("\n");
	}

	private void emptyTagLn(StringBuffer result,String tag, String attributes)
	{
		if (createTags) result.append("<").append(tag).append(" ").append(attributes).append("/>");
		result.append("\n");
	}

    private LinkedList<String> getCategoryTags(HashSet<String> categories)
    {
        HashSet<String> conceptSet=new HashSet<String>();
        
        Iterator<String> cit=categories.iterator();
        for (int i=0;i<categories.size();i++)
        {
            String c=cit.next();
            // convert the category name to the internal format
            
            String cname="wikicategory_"+c;
            cname=cname.replaceAll(" ","_");

            // there are some categories with '|', we ignore the suffix part
            
            if (cname.indexOf('|')!=-1)
                cname=cname.substring(0,cname.indexOf('|'));
            
//            System.out.println("found category "+c+"\n\tcorresponds to internal category "+cname);
            
            // probe the database
            
            try
            {
                readWikiFacts.setString(1, cname);
                ResultSet rs=readWikiFacts.executeQuery();
                while (rs.next())
                {
                    String coname=rs.getString(1);
//                    System.out.println("\tread concept "+coname);
                    String concept=coname.substring(coname.indexOf("_")+1);
                    concept=concept.substring(0,concept.indexOf("_"));
//                    System.out.println("\tfinal tag: "+concept);
                    conceptSet.add(concept);
                }
                rs.close();
                
            }
            catch(Exception e)
            {
                problem("Huh? Exception:\n"+e);
            }
        }
        LinkedList<String> categoryTags=new LinkedList<String>(conceptSet);

        return categoryTags;
    }
    
    private class pair
    {
        String concept;
        String id;
        
        public pair(String c, String i)
        {
            concept=c;
            id=i;
        }
        
        public boolean equals(Object o)
        {
            if (o instanceof pair)
            {
                pair p=(pair)o;
                return ((concept.equals(p.concept)&&(id.equals(p.id))));
            }
            else return false;
        }
        
        public int hashCode()
        {
            return concept.hashCode();
        }
        
        public String toString()
        {
            return concept+"["+id+"]";
        }
    }
    
    private void collectConcepts(HashSet<String> categories)
    {
        
        HashSet<pair> conceptSet=new HashSet<pair>();
        
        Iterator<String> cit=categories.iterator();
        for (int i=0;i<categories.size();i++)
        {
            String c=cit.next();
            // convert the category name to the internal format
            
            String cname="wikicategory_"+c;
            cname=cname.replaceAll(" ","_");

            // there are some categories with '|', we ignore the suffix part
            
            if (cname.indexOf('|')!=-1)
                cname=cname.substring(0,cname.indexOf('|'));

//            System.out.println("found category "+c+"\n\tcorresponds to internal category "+cname);
            
            // probe the database
            
            try
            {
                readWikiFacts.setString(1, cname);
                ResultSet rs=readWikiFacts.executeQuery();
                while (rs.next())
                {
                    String coname=rs.getString(1);
//                    System.out.println("\tread concept "+coname);
                    String concept=coname.substring(coname.indexOf("_")+1);
                    String idstring=concept.substring(concept.lastIndexOf('_')+2);
                    concept=concept.substring(0,concept.lastIndexOf("_"));
//                    System.out.println("\tfinal tag: "+concept+"\t id="+idstring);
                    conceptSet.add(new pair(concept,idstring));
                }
                rs.close();
            }
            catch(Exception e)
            {
                System.out.println("Huh? Exception:n"+e);
            }
        }

        if (conceptSet.size()>0)
        {
            LinkedList<pair> categoryTags=new LinkedList<pair>(conceptSet);
         
            try
            {
                ListIterator<pair> it=categoryTags.listIterator();
                for (int i=categoryTags.size();i>0;i--)
                {
                    pair c=it.next();
//                    System.out.println("\t"+c);
                    writePageConceptMapping.setString(1,makeWikiURL(currentTitle));
                    writePageConceptMapping.setString(2,c.concept);
                    writePageConceptMapping.setString(3, c.id);
                    writePageConceptMapping.addBatch();
                }
                
                writePageConceptMapping.executeBatch();
            }
            catch(Exception e)
            {
                System.out.println("cannot write concept mapping for "+currentTitle+":\n"+e);
            }
        }
    }
    
    // support methods for annotations
    
    // label from ClusteredListReader, but extended by graph features
    
    private class label
    {
        String concept;
        int id;
        
        double confidence=1.0; //default, may be overridden by constructor
        
        String lineage;
        
        HashSet<label> parents;
        HashSet<label> children;
        
        public label(String c, int i)
        {
            concept=c;
            id=i;
        }
        
        public label (String c, int i, double conf)
        {
            this(c,i);
            confidence=conf;
            parents=new HashSet<label>();
            children=new HashSet<label>();
        }
        
        public boolean equals(Object o)
        {
            if (o instanceof label)
            {
                label p=(label)o;
                return ((concept.equals(p.concept)&&(id==p.id)));
            }
            else return false;
        }
        
        public int hashCode()
        {
            return concept.hashCode();
        }
        
        public String toString()
        {
            return concept+"["+id+"]@"+confidence;
        }
    }

    // 1:1 from ClusteredListReader
    
    private class synsetComparator implements Comparator<ISynset>
    {
        public int compare(ISynset s1, ISynset s2)
        {
            return Integer.signum((int)(s1.getOffset()-s2.getOffset()));
        }

    }

    private HashSet<label> readAndConsolidateLabels(String url)
    {
        url=url.replaceAll("\\s", "_");
        
//        System.out.println("rACL("+url+")");
        
        HashSet<label> readLabels=new HashSet<label>();
     
        // read all labels
        
        if (haveLocalAnnotations)
        {
        	Wiki2XML.annotation ann[]=annotations.get(url);
        	if (ann!=null)
        	{
        		for (int i=0;i<ann.length;i++)
            	{
                    label l=new label(id2concept.get(ann[i].conceptid),ann[i].conceptid,ann[i].confidence);
                    l.lineage="yago";
                    readLabels.add(l);            		
            	}
        	}
        }
        else
        {
            try
            {
            	
                readPageConceptMapping.setString(1,url);
                ResultSet rs=readPageConceptMapping.executeQuery();
                while (rs.next())
                {
                    String concept=rs.getString(1);
                    String conceptid=rs.getString(2);
                    double confidence=rs.getDouble(3);
                    
                    label l=new label(concept,Integer.parseInt(conceptid),confidence);
                    l.lineage="categories";
                    readLabels.add(l);
                }
                rs.close();

                if ((createNewAnnotations)&&(url.startsWith("../19")==false)&&(url.startsWith("../20")==false))
                {
                    readNewConcepts.setString(1,url);
                    rs=readNewConcepts.executeQuery();
                    while (rs.next())
                    {
                        String fullconcept=rs.getString(1);
                        double confidence=rs.getDouble(2);
                        String numUrls=String.valueOf(rs.getInt(3));
                        
                        // extract concept term from full concept
                        
                        String concept=fullconcept.substring(0,fullconcept.lastIndexOf('_'));
                        String conceptid=fullconcept.substring(fullconcept.lastIndexOf('_')+1);
                        concept=concept.substring(new String("wordnet_").length());
                        
                        label l=new label(concept,Integer.parseInt(conceptid),confidence);
                        l.lineage=numUrls+" lists";
                        readLabels.add(l);
                    }
                    rs.close();
                }
            }
            catch(Exception e)
            {
                System.out.println("cannot check concept database for "+url+":\n"+e);
            }        	
        }
        
        HashSet<label> labels=readLabels;
        
        if (minimizeLabels)
        {        	
	        labels=minimizeLabels(readLabels);
	        
	        labels=consolidateLabels(labels);
	        
	        labels=minimizeLabels(labels);
        }
        return labels;
    }

    private HashSet<label> readAndConsolidateCategoryLabels(HashSet<String> categories, HashSet<label> readLabels)
    {
        // read all labels
        
        if (haveLocalAnnotations)
        {
        	Iterator<String> it=categories.iterator();
        	while (it.hasNext())
        	{
        		String category=it.next();
        		category=category.replaceAll("\\s", "_");
            	Wiki2XML.annotation ann[]=catAnnotations.get(category);
            	if (ann!=null)
            	{
            		for (int i=0;i<ann.length;i++)
                	{
                        label l=new label(id2concept.get(ann[i].conceptid),ann[i].conceptid,ann[i].confidence);
                        l.lineage="yago";
                        readLabels.add(l);            		
                	}
            	}
        	}
        }
        HashSet<label> labels=readLabels;
        
        if (minimizeLabels)
        {        	
	        labels=minimizeLabels(readLabels);
	        
	        labels=consolidateLabels(labels);
	        
	        labels=minimizeLabels(labels);
        }
        return labels;
    }

    private HashMap<label,HashSet<label>> incompatibilities;

    private HashSet<label> consolidateLabels(HashSet<label> readLabels)
    {
//        System.out.println("consolidateLabels() for "+readLabels.size()+" labels");
        
        HashSet<label> labels=null;

        HashMap<label,HashSet<label>> map=null;
        
        // build the complete set of labels (including ancestors in the WordNet tree)
        // until there are no incompatibilities left
        
        boolean incompatibilitiesFound=true;
        
        while (incompatibilitiesFound)
        {
            labels=new HashSet<label>();
            map=new HashMap<label,HashSet<label>>();
            
            Iterator<label> it=readLabels.iterator();
            for (int i=readLabels.size();i>0;i--)
            {
                label la=it.next();
                
//                System.out.println("consolidate(): "+la);
                
                ISynset syn=null;
                
                try
                {
                	syn=dict.getSynset(new SynsetID(la.id,POS.NOUN));
//                    syn=dict.getSynsetAt(POS.NOUN, la.id);
                }
                catch(Exception e)
                {
                    System.out.println("cannot read synset for "+la.concept+"["+la.id+"]:\n"+e);
                }
                
                TreeSet<ISynset> synsets=new TreeSet<ISynset>(new synsetComparator());
                synsets.add(syn);
                
                while (synsets.size()!=0)
                {
                    syn=synsets.first();
                    synsets.remove(syn);
                    
//                    System.out.println("-->"+syn+"<-");
                    
                    try
                    {
                        // find the corresponding label
                        
                        String term=syn.getWord(1).getLemma();
                        
//                        System.out.println("current term: "+term);
                        
                        label np=new label(term,(int)syn.getOffset(),la.confidence);
                        np.lineage=la.lineage;
                        
                        labels.add(np);
                        
                        if (baseTypes.contains(np))
                        {
                            HashSet<label> set=map.get(np);
                            if (set==null)
                            {
                                set=new HashSet<label>();
                                map.put(np,set);
                            }
                            set.add(la);
//                            System.out.println("add "+la+" to "+np);
                        }
                    }
                    catch(Exception e)
                    {
                        System.out.println("strange exception:\n"+e);
                        e.printStackTrace();
                    }

                    // move up the wordnet tree and collect other synsets

                    List<ISynsetID> pointers=syn.getRelatedSynsets(Pointer.HYPERNYM);
                    ListIterator<ISynsetID> pit=pointers.listIterator();
                    for (int k=0;k<pointers.size();k++)
                    {
                        try
                        {
                        	ISynsetID sid=pit.next();
                            ISynset s=(ISynset)dict.getSynset(sid);
                            synsets.add(s);
                        }
                        catch(Exception e)
                        {
                            System.out.println("strange exception:\n"+e);
                            e.printStackTrace();
                        }
                    }
                    
                }
            }
            // now check for incompatibilities
            
            HashMap<label,HashSet<label>> numIncomp=new HashMap<label,HashSet<label>>();
            Iterator<Map.Entry<label, HashSet<label>>> typeit=map.entrySet().iterator();
            while (typeit.hasNext())
            {
                Map.Entry<label,HashSet<label>> e=typeit.next();
                
                Iterator<label> confit=incompatibilities.get(e.getKey()).iterator();
                while (confit.hasNext())
                {
                    label c=confit.next();
                    if (map.containsKey(c))
                    {
//                        System.out.println("found incomp: "+e.getKey()+"<->"+c+" ["+map.get(c).size()+"]");
                        HashSet<label> conflabels=numIncomp.get(e.getKey());
                        if (conflabels==null)
                        {
                            conflabels=new HashSet<label>();
                            numIncomp.put(e.getKey(),conflabels);
                        }
                        conflabels.addAll(map.get(c));
                        // in case a label is mapped to two different base types,
                        // remove it again as we don't count these cases
                        conflabels.removeAll(map.get(e.getKey()));
                    }
                }
            }
            
            if (numIncomp.size()>0)
            {
                // there was at least one incompatibility, so try to find
                // the type with most incompatibilities
                // (that's a heuristic to find the best set of nonconflicting labels)
                
                int maxConfs=0;
                int minConfs=Integer.MAX_VALUE;
                
                label maxConcept=null;
                
                Iterator<Map.Entry<label,HashSet<label>>> mit=numIncomp.entrySet().iterator();
                while (mit.hasNext())
                {
                    Map.Entry<label,HashSet<label>> e=mit.next();
                    int s=e.getValue().size();
//                    System.out.println("check: "+e.getKey()+"->"+s);
                    if (s>maxConfs)
                    {
                        maxConfs=s;
                        maxConcept=e.getKey();
                    }
                    
                    if (s<minConfs) minConfs=s;
                }
                
                if (maxConcept!=null)
                {
                    if (maxConfs==minConfs)
                    {
//                        System.out.println("there is no single max type, abort.");
                        incompatibilitiesFound=false;
                        break;
                    }
                    
                    // remove all labels that belong to the maximally conflicting type
                    // and restart the procedure
                    
//                    System.out.println("max conflicting type is "+maxConcept+" ["+maxConfs+" conflicts, min="+minConfs+"], remove it.");
                    readLabels.removeAll(map.get(maxConcept));
                    labels.removeAll(map.get(maxConcept));
                    //map.remove(maxConcept);
                }
                else
                {
//                    System.out.println("Huh? Something broken, abort.");
                    break;
                }
            }
            else
                incompatibilitiesFound=false;
        }
        
//        System.out.println("return "+labels);
        
        return labels;
    }
    
    // cut down the label tree
    
    private HashSet<label> minimizeLabels(HashSet<label> labels)
    {
        System.out.println("minimizeLabels() for "+labels.size()+" labels");
        System.out.println(labels);
        
        HashSet<label> result=new HashSet<label>();
        
        // first round: collect all labels
        
        HashSet<label> newlabels=new HashSet<label>();
        
        Iterator<label> it=labels.iterator();
        for (int i=labels.size();i>0;i--)
        {
            label la=it.next();
            
//            System.out.println("consolidate(): "+la);
            
            ISynset syn=null;
            
            try
            {
            	if (la.id>100000000) syn=dict.getSynset(new SynsetID(la.id-100000000,POS.NOUN));
            	else syn=dict.getSynset(new SynsetID(la.id,POS.NOUN));
//                syn=dict.getSynsetAt(POS.NOUN, la.id-100000000);
            }
            catch(Exception e)
            {
                System.out.println("cannot read synset for "+la.concept+"["+la.id+"]:\n"+e);
            }
            
            TreeSet<ISynset> synsets=new TreeSet<ISynset>(new synsetComparator());
            synsets.add(syn);
            
            while (synsets.size()!=0)
            {
                syn=synsets.first();
                synsets.remove(syn);
                
//                System.out.println("-->"+syn+"<-");
                
                try
                {
                    // find the corresponding label
                    
                	String term=syn.getWord(1).getLemma();
//                    String term=syn.getWords()[0].getLemma();
                    
//                    System.out.println("current term: "+term);
                    
                    label np=new label(term,(int)syn.getOffset(),la.confidence);
                    np.lineage=la.lineage;
                    
                    newlabels.add(np);
                    
                }
                catch(Exception e)
                {
                    System.out.println("strange exception:\n"+e);
                    e.printStackTrace();
                }

                // move up the wordnet tree and collect other synsets

                List<ISynsetID> pointers=syn.getRelatedSynsets(Pointer.HYPERNYM);
                ListIterator<ISynsetID> pit=pointers.listIterator();
                for (int k=0;k<pointers.size();k++)
                {
                    try
                    {
                    	ISynsetID sid=pit.next();
                        ISynset s=(ISynset)dict.getSynset(sid);
                        synsets.add(s);
                    }
                    catch(Exception e)
                    {
                        System.out.println("strange exception:\n"+e);
                        e.printStackTrace();
                    }
                }
                
            }
        }

        // second round: add edges
        
        List<label> labellist=new LinkedList<label>(newlabels);        

        it=labellist.iterator();
        for (int i=labellist.size();i>0;i--)
        {
            label la=it.next();
            
//            System.out.println("consider label "+la);
            ISynset syn=null;
            
            try
            {
            	syn=dict.getSynset(new SynsetID(la.id,POS.NOUN));
//                syn=dict.getSynsetAt(POS.NOUN, la.id);
            }
            catch(Exception e)
            {
                System.out.println("cannot read synset for "+la.concept+"["+la.id+"]:\n"+e);
            }
            
            TreeSet<ISynset> synsets=new TreeSet<ISynset>(new synsetComparator());
            synsets.add(syn);
            
            while (synsets.size()!=0)
            {
                syn=synsets.first();
                synsets.remove(syn);
                
                String synterm=syn.getWord(1).getLemma();
                int synid=(int)syn.getOffset();
                
                label synlabel=findLabel(labellist,synterm,synid);
                
                // move up the wordnet tree and collect other synsets

                List<ISynsetID> pointers=syn.getRelatedSynsets(Pointer.HYPERNYM);
                ListIterator<ISynsetID> pit=pointers.listIterator();
                for (int k=0;k<pointers.size();k++)
                {
                    try
                    {
                    	ISynsetID sid=pit.next();
                        ISynset s=(ISynset)dict.getSynset(sid);
                        synsets.add(s);
                        
                        String term=s.getWord(1).getLemma();
                        long id=s.getOffset();
                        label l=findLabel(labellist,term,(int)id);
                        l.children.add(synlabel); 
                        synlabel.parents.add(l); // probably not necessary
                    }
                    catch(Exception e)
                    {
                        System.out.println("strange exception:\n"+e);
                        e.printStackTrace();
                    }
                }
            }
        }
        
        // simple minimization: keep only sinks of the graph (i.e., labels where children.size()==0))
        
        it=labellist.iterator();
        while (it.hasNext())
        {
            label l=it.next();
            
//            System.out.println("min: consider label "+l+", #children="+l.children.size());
            if (l.children.size()==0) result.add(l);
        }


        System.out.println("return "+result.size()+" results");
        System.out.println(result);
        return result;
    }
    
    private label findLabel(List<label> l, String t, int id)
    {
//        System.out.println("findLabel("+t+","+id+")");
        
        label result=null;
        
        ListIterator<label> it=l.listIterator();
        while (it.hasNext())
        {
            label la=it.next();
            if ((la.concept.equals(t))&&(la.id==id))
            {
                result=la;
                break;
            }
        }
        return result; 
    }
    
    private HashSet<label> baseTypes;
    
    final label LIVING_THING=new label("living_thing",3009);
    final label WHOLE=new label("whole",2645);
    final label THING=new label("thing",4253302);
    final label ACT=new label("act",26194);
    final label SUBSTANCE=new label("substance",17572);
    final label LOCATION=new label("location",22625);
    final label CAUSAL_AGENT=new label("causal_agent",5598);
    final label ABSTRACTION=new label("abstraction",20486);
    final label STATE=new label("state",24568);
    final label GROUP=new label("group",26769);
    final label EVENT=new label("event",25950);
    final label POSSESSION=new label("possession",27371);
    
    private void initIncompatibilities()
    {
        // list of the base types
        
        baseTypes=new HashSet<label>();
        baseTypes.add(LIVING_THING);
        baseTypes.add(WHOLE);
        baseTypes.add(THING);
        baseTypes.add(SUBSTANCE);
        baseTypes.add(LOCATION);
        baseTypes.add(CAUSAL_AGENT);
        baseTypes.add(ACT);
        baseTypes.add(ABSTRACTION);
        baseTypes.add(STATE);
        baseTypes.add(GROUP);
        baseTypes.add(EVENT);
        baseTypes.add(POSSESSION);
        
        incompatibilities=new HashMap<label,HashSet<label>>();
        
        // init incompatible base types
        
        // usage of this table: the list for an entry shows all other types that are incompatible
        // to this entry (and therefore should not be assigned to the same instance)
        
        // living_thing (subclass of object, which is in turn a subclass of entity)
        HashSet<label> s=new HashSet<label>();
        s.add(WHOLE);
        s.add(SUBSTANCE);
        s.add(STATE);
        s.add(THING);
        s.add(LOCATION);
        s.add(EVENT);
        s.add(ABSTRACTION); // unclear
        s.add(ACT);
        s.add(POSSESSION);
        incompatibilities.put(LIVING_THING, s);
        
        // thing & whole (seem to be similar classes)
        s=new HashSet<label>();
        s.add(LIVING_THING);
        s.add(SUBSTANCE);
        s.add(STATE);
        s.add(LOCATION);
        s.add(EVENT);
        s.add(ABSTRACTION); 
        s.add(ACT);
        s.add(POSSESSION);
        s.add(CAUSAL_AGENT);
        s.add(GROUP);
        incompatibilities.put(THING, s);
        incompatibilities.put(WHOLE, s);

        // substance 
        s=new HashSet<label>();
        s.add(LIVING_THING);
        s.add(WHOLE);
        s.add(THING);
        s.add(STATE);
        s.add(LOCATION);
        s.add(EVENT);
        s.add(ABSTRACTION);
        s.add(ACT);
        s.add(POSSESSION);
        s.add(GROUP);
        s.add(CAUSAL_AGENT);
        incompatibilities.put(SUBSTANCE, s);

        // location
        s=new HashSet<label>();
        s.add(LIVING_THING);
        s.add(WHOLE);
        s.add(THING);
        s.add(STATE);
        s.add(SUBSTANCE);
        s.add(EVENT);
        s.add(ABSTRACTION);
        s.add(ACT);
        s.add(POSSESSION);
        s.add(GROUP);
        s.add(CAUSAL_AGENT);
        incompatibilities.put(LOCATION, s);
        
        // causal_agent
        s=new HashSet<label>();
        s.add(WHOLE);
        s.add(THING);
        s.add(STATE);
        s.add(LOCATION);
        s.add(EVENT);
        s.add(ABSTRACTION);
        s.add(ACT);
        s.add(POSSESSION);
//        s.add(GROUP);
        incompatibilities.put(CAUSAL_AGENT, s);

        // group
        s=new HashSet<label>();
        s.add(WHOLE);
        s.add(THING);
        s.add(STATE);
        s.add(LOCATION);
        s.add(SUBSTANCE);
        s.add(EVENT);
        s.add(ABSTRACTION);
        s.add(ACT);
        s.add(POSSESSION);
        incompatibilities.put(GROUP, s);

        // abstraction (incompatible with all other base types)
        s=new HashSet<label>();
        s.add(WHOLE);
        s.add(THING);
        s.add(LIVING_THING);
        s.add(CAUSAL_AGENT);
        s.add(LOCATION);
        s.add(SUBSTANCE);
        s.add(STATE);
        s.add(ACT);
        s.add(POSSESSION);
        s.add(GROUP);
        incompatibilities.put(ABSTRACTION, s);

        // act (incompatible with all other base types)
        s=new HashSet<label>();
        s.add(WHOLE);
        s.add(THING);
        s.add(LIVING_THING);
        s.add(CAUSAL_AGENT);
        s.add(LOCATION);
        s.add(SUBSTANCE);
        s.add(STATE);
        s.add(EVENT);
        s.add(ABSTRACTION);
        s.add(POSSESSION);
        s.add(GROUP);
        incompatibilities.put(ACT, s);

        // state (incompatible with all other base types)
        s=new HashSet<label>();
        s.add(WHOLE);
        s.add(THING);
        s.add(LIVING_THING);
        s.add(LOCATION);
        s.add(CAUSAL_AGENT);
        s.add(SUBSTANCE);
        s.add(EVENT);
        s.add(ABSTRACTION);
        s.add(ACT);
        s.add(POSSESSION);
        s.add(GROUP);
        incompatibilities.put(STATE, s);

        // state (incompatible with all other base types)
        s=new HashSet<label>();
        s.add(WHOLE);
        s.add(THING);
        s.add(LIVING_THING);
        s.add(LOCATION);
        s.add(CAUSAL_AGENT);
        s.add(SUBSTANCE);
        s.add(STATE);
        s.add(ACT);
        s.add(POSSESSION);
        s.add(GROUP);
        incompatibilities.put(EVENT, s);

        // state (incompatible with all other base types)
        s=new HashSet<label>();
        s.add(WHOLE);
        s.add(THING);
        s.add(LIVING_THING);
        s.add(LOCATION);
        s.add(CAUSAL_AGENT);
        s.add(SUBSTANCE);
        s.add(EVENT);
        s.add(ABSTRACTION);
        s.add(ACT);
        s.add(STATE);
        s.add(GROUP);
        incompatibilities.put(POSSESSION, s);
    }

    private String openAnnotationTags(List<label> annotations)
    {
        StringBuffer result=new StringBuffer(200);
        
        Iterator<label> it=annotations.iterator();
        while (it.hasNext())
        {
            label l=it.next();
            result.append("<").append(l.concept).append(" confidence=\"").append(l.confidence).append("\" wordnetid=\"").append(l.id).append("\" source=\"").append(l.lineage).append("\">\n");
        }
        
        return result.toString();
    }

    private void openAnnotationTags(List<label> annotations, StringBuffer res, boolean inHeader)
    {
        Iterator<label> it=annotations.iterator();
        while (it.hasNext())
        {
            label l=it.next();
            if (inHeader) openHeaderTagLn(res,l.concept," confidence=\""+l.confidence+"\" wordnetid=\""+l.id+"\"");
            else openTagLn(res,l.concept," confidence=\""+l.confidence+"\" wordnetid=\""+l.id+"\"");
        }
    }

    private String closeAnnotationTags(List<label> annotations)
    {
        StringBuffer result=new StringBuffer(200);
        
        ListIterator<label> it=annotations.listIterator(annotations.size());
        while (it.hasPrevious())
        {
            label l=it.previous();
            result.append("</").append(l.concept).append(">\n");
        }
        
        return result.toString();
    }

    private void closeAnnotationTags(List<label> annotations, StringBuffer res)
    {
        ListIterator<label> it=annotations.listIterator(annotations.size());
        while (it.hasPrevious())
        {
            label l=it.previous();
            closeTagLn(res,l.concept);
        }
    }

    private String replacePipes(String r)
    {
    	if (r==null) return "";
    	char arr[]=r.toCharArray();
    	
    	if (arr==null) return "";
    	
    	int open=0;
    	for (int i=0;i<arr.length;i++)
    	{
    		if (arr[i]=='{') open++;
    		else if (arr[i]=='}') open--;
    		else if ((arr[i]=='|')&&(open==0))
    			arr[i]='�';
    	}
    	return new String(arr);
    }
    
    private String getPipesBack(String r)
    {
    	if (r==null) return "";
    	char arr[]=r.toCharArray();
    
    	if (r.length()==0)
    		System.out.println("oops");
    	
    	if (arr==null) return "";
    	
    	for (int i=0;i<arr.length;i++)
    	{
    		if (arr[i]=='�')
    			arr[i]='|';
    	}
    	return new String(arr);
    }
    
    private HashMap<String,String> templates;
    private HashMap<String,String> templateNames;
    
    static final String CLIX_TEMPLATE="__CLIX_TEMPLATE_";
    int lasttemplateid=0;
    
    private String expandTemplate(String templatecall)
    {
    	String res="";
    	String data="";
    	
    	String name="";
    	
    	if (templatecall.startsWith("[[")) return templatecall;
    	
    	if (templatecall.startsWith("''")) // broken call in Axiom of regularity
    	{
    		return "<nowiki>"+templatecall+"</nowiki>";
    	}
    	int idx=templatecall.indexOf('|');
    	if (idx==-1) name=templatecall;
    	else name=templatecall.substring(0,idx);
    	
    	String lname=name.toLowerCase().trim();
    	
    	if (keepOnlyDataTemplates.contains(lname))
    	{
    		if ((idx==-1)||(idx==templatecall.length()-1)) return "";
    		else return templatecall.substring(idx+1);
    	}
    	
    	if (forbiddenTemplates.contains(lname))
    	{
//    		System.out.println("ignored: "+name);
    		return "";
    	}
    	else
    	{
    		Iterator<String> it=forbiddenTemplatePrefixes.iterator();
    		for (int i=0;i<forbiddenTemplatePrefixes.size();i++)
    		{
    			if (lname.startsWith(it.next()))
    			{
//    	    		System.out.println("ignored: "+name);
    	    		return "";
    			}
    		}

    		it=forbiddenTemplateInfixes.iterator();
    		for (int i=0;i<forbiddenTemplateInfixes.size();i++)
    		{
    			if (lname.indexOf(it.next())!=-1)
    			{
//    	    		System.out.println("ignored: "+name);
    	    		return "";
    			}
    		}

    	}
    	if ((idx!=-1)&&(xmlTemplates.contains(lname)))
    	{
//    		System.out.println("xmlify: "+name);
    		
    		String id=Integer.toString(++lasttemplateid);
    		templates.put(id, templatecall.substring(idx+1));
    		templateNames.put(id, name);
    		return "";//CLIX_TEMPLATE+id+"_";
    	}
    	else if (idx!=-1)
    	{
    		Iterator<String> it=xmlTemplatePrefixes.iterator();
    		for (int i=0;i<xmlTemplatePrefixes.size();i++)
    		{
    			if (lname.startsWith(it.next()))
    			{
//    	    		System.out.println("xmlify: "+name);
    	    		
    	    		String id=Integer.toString(++lasttemplateid);
    	    		templates.put(id, templatecall.substring(idx+1));    	    		
    	    		templateNames.put(id, name);
//    	    		return CLIX_TEMPLATE+id+"_";    				
    	    		return ""; // we embed the template call directly at the beginning of the generated xml    				
    			}
    		}
    	}
    	if (parserTemplates.contains(lname))
    	{
//    		System.out.println("interpret: "+name);
    		
			String r=handleTemplate(templatecall);

			return removeOnlyComments(handleInternalTemplates(r));
    	}
    	OutputStreamWriter wr=null;
    	
    	BufferedReader rd=null;
    	
    	try
    	{
//    		System.out.println("expand (server): "+name);
    		
    		// quick check for malformed cite template
    		
    		if (((templatecall.startsWith("cite"))||(templatecall.startsWith("Citation"))||(templatecall.startsWith("citation")))&&(templatecall.indexOf("''")!=-1))
    		{
//    			problem("FIX: "+templatecall);
//    			int bidx=templatecall.indexOf("'' |");
//    			while (bidx!=-1)
//    			{
//    				templatecall=templatecall.substring(0, bidx)+" |"+templatecall.substring(bidx+"'' |".length());
//        			bidx=templatecall.indexOf("'' |");
//    			}
//    			bidx=templatecall.indexOf("''|");
//    			while (bidx!=-1)
//    			{
//    				templatecall=templatecall.substring(0, bidx)+"|"+templatecall.substring(bidx+"''|".length());
//        			bidx=templatecall.indexOf("''|");
//    			}
    			templatecall=templatecall.replaceAll("''", "");
//    			System.out.println("POST FIX: "+templatecall);
    		}
        	URL url=new URL(Wiki2XML.templateExpanderURI);
        	
            // Construct data
            data = URLEncoder.encode("input", "UTF-8") + "=" + URLEncoder.encode("{{"+templatecall+"}}", "UTF-8");
            data+="&contexttitle="+URLEncoder.encode(currentTitle, "UTF-8");
            
            // Send data
            URLConnection conn = url.openConnection();
            conn.setDoOutput(true);
            
            wr = new OutputStreamWriter(conn.getOutputStream());
            wr.write(data);
            wr.flush();
        
//            System.out.println("data= "+data);
            // Get the response


            rd = new BufferedReader(new InputStreamReader(conn.getInputStream(),"UTF-8"));
            String line;
            while ((line = rd.readLine()) != null) {
//            	System.out.println("line="+line);
                // Process line...
            	res+=line+"\n";
            }
            if (res.length()>0) res=res.substring(0,res.length()-1); // remove final linefeed
            
//            if (name.compareTo("dmoz")==0)
//            {
//            	System.out.println(data);
//            	System.out.println(res);
//            }
            
            if (conn instanceof HttpURLConnection)
            {
            	HttpURLConnection httpc=(HttpURLConnection) conn;
            	httpc.disconnect();            	
            }
    	}
    	catch(Exception e)
    	{
    		System.err.println("cannot convert "+templatecall+":");
    		System.err.println("data="+data);
    		System.err.println("res="+res);
    		e.printStackTrace();    		
    	}
    	finally
    	{
    		try
    		{
    			if (wr!=null) wr.close();
        		if (rd!=null) rd.close();
    		}
    		catch(Exception e)
    		{
    			System.err.println("exception during finally clause of expandTemplate()");
    		}
    	}

    	res=res.replaceAll("&gt;", ">");
    	res=res.replaceAll("&lt;", "<");
    	res=res.replaceAll("&quot;", "\"");
    	
    	String tmp="{{"+templatecall+"}}";
    	if (tmp.compareTo(res)==0)
    	{
    		problem("received the original template back!");
    		res="";
    	}

    	if (res.startsWith("{{"))
    	{
    		problem("expandTemplate("+name+") got unexpected template call back from server");
    		res="";
    	}

//        System.out.println("res= "+res);

    	res=removeOnlyComments(res);

//        System.out.println("res= "+res);

        return res;
    }

    private void handleContent(String c, StringBuffer res)
    {
    	handleContent(c,0,c.length(),res);
    }
    
    private final static int OP_NOOP=-1;
    private final static int OP_TABLE=0;
    private final static int OP_SECTION=1;
    private final static int OP_EMPHASIS=2;
    private final static int OP_LINK=3;
    private final static int OP_WEBLINK=4;
    private final static int OP_LIST=5;
    private final static int OP_DOUBLELF=6;
    private final static int OP_INDENT=7;
    
    private boolean haveParagraph=false;
    
    // end is the first index past the substring that should be considered
    //  this method should have the same result as first computing s=content.substring(start,end)
    //  amd then calling it as handleContent(s,0,s.length(),res)
    
    private void handleContent(String content, int pos, int end, StringBuffer res)
    {
    	// shortcut: if there's nothing to parse, don't parse.
    	if (content.length()==0) return;
    	
    	boolean haveBoldCache=haveBold;
    	boolean haveItalicsCache=haveItalics;
    	boolean italicsFirstCache=italicsFirst;
    	
    	boolean haveParagraphCache=haveParagraph;
    	
    	haveBold=false;
    	haveItalics=false;
    	italicsFirst=false;
    	
    	haveParagraph=false;
    	
    	boolean openHeadersCache[]=new boolean[openHeaders.length];
    	for (int i=0;i<openHeaders.length;i++)
    	{
        	openHeadersCache[i]=openHeaders[i];
        	openHeaders[i]=false;    		
    	}
    	// test: remove this (maybe we get better spaces in the generated text)
    	
//        while ((pos<end)&&(Character.isSpaceChar(content.charAt(pos)))) pos++;

        // we have to add a trailing linefeed to be able to match lists, indents, etc
  
    	if ((pos<end)&&(content.charAt(pos)!='\n'))
    	{
            content="\n"+content.substring(pos);
            end=end-pos+1;
            pos=0;    		
    	}
        
    	while ((pos>-1)&&(pos<end))
    	{
    	    int nextOp=OP_NOOP;
    	    int minidx=Integer.MAX_VALUE;
    	    // tables
    	    
    	    int nextTable=content.indexOf("{|", pos);
    	    if ((nextTable>-1)&&(nextTable+1<end))
    	    {
    	        nextOp=OP_TABLE;
    	        minidx=nextTable;
    	    }
    	    
    	    // section headers
    	    int nextSection=content.indexOf("\n==", pos);
    	    if ((nextSection>-1)&&(nextSection+2<end)&&(nextSection+1<minidx))
    	    {
    	        minidx=nextSection+1;
    	        nextOp=OP_SECTION;
    	    }

    	    // emphasis
    	    int nextEmphasis=content.indexOf("''", pos);
    	    if ((nextEmphasis>-1)&&(nextEmphasis+2<end)&&(nextEmphasis<minidx))
    	    {
    	        minidx=nextEmphasis;
    	        nextOp=OP_EMPHASIS;
    	    }
    	    
    	    // links
            int nextLink=content.indexOf("[[", pos);
            if ((nextLink>-1)&&(nextLink+2<end)&&(nextLink<minidx))
            {            	
                minidx=nextLink;
                nextOp=OP_LINK;
            }

            // weblinks
            int nextWebLink=content.indexOf("[", pos);
            if ((nextWebLink>-1)&&(nextWebLink<end)&&(nextWebLink<minidx))
            {
                minidx=nextWebLink;
                nextOp=OP_WEBLINK;
            }

            // lists
            int nextUList=content.indexOf("\n*", pos);
            if ((nextUList>-1)&&(nextUList+2<end)&&(nextUList+1<minidx))
            {
                minidx=nextUList+1;
                nextOp=OP_LIST;
            }

            int nextNList=content.indexOf("\n#", pos);
            if ((nextNList>-1)&&(nextNList+2<end)&&(nextNList+1<minidx))
            {
                minidx=nextNList+1;
                nextOp=OP_LIST;
            }

            int nextDList=content.indexOf("\n;", pos);
            if ((nextDList>-1)&&(nextDList+2<end)&&(nextDList+1<minidx))
            {
                minidx=nextDList+1;
                nextOp=OP_LIST;
            }

            int nextIndent=content.indexOf("\n:", pos);
            if ((nextIndent>-1)&&(nextIndent+2<end)&&(nextIndent+1<minidx))
            {
                minidx=nextIndent+1;
                nextOp=OP_INDENT;
            }

            int nextDLF=content.indexOf("\n\n", pos);
            if ((nextDLF>-1)&&(nextDLF+2<end)&&(nextDLF<minidx))
            {
                minidx=nextDLF;
                nextOp=OP_DOUBLELF;
            }

    	    if (minidx<Integer.MAX_VALUE) res.append(XML.XMLify2(content.substring(pos,minidx))); 
    	    
    	    switch(nextOp)
    	    {
    	    case OP_TABLE:
    	        int endidx=nextTable+2;
    	        int tablecnt=1;
    	        
    	        while ((endidx<end)&&(tablecnt>0))
    	        {
    	            if ((content.substring(endidx).startsWith("{|")))
    	                tablecnt++;
    	            else if ((content.substring(endidx).startsWith("|}")))
    	                tablecnt--;
    	            endidx++;
    	        }
    	        if (endidx>=end)
    	        {
    	            problem("broken table definition (unbalanced open and close tags), ignore.");
    	            pos+=2;
    	            continue;
    	        }
    	        handleTable(content,minidx+2,endidx,res);
    	        pos=endidx+1;
    	        break;
    	    
    	    case OP_SECTION:
    	        // count number of initial equals 
    	        int cnteq=0; int spos=minidx;
    	        while ((spos<end)&&(content.charAt(spos)=='=')) {spos++; cnteq++;}
    	        int initialcnteq=cnteq;
    	        
    	        while ((spos<end)&&(cnteq>0))
    	        {
    	            if (content.charAt(spos++)=='=') cnteq--;
    	        }
    	        if (cnteq>0)
    	        	System.out.println("Huh?");
    	        
    	        endidx=spos-initialcnteq;
    	        
    	        handleHeader(content,minidx+initialcnteq,endidx,initialcnteq,res);
    	        pos=spos; // one char after the last '='
    	        break;
    	    
    	    case OP_EMPHASIS:
                // count number of initial ' characters 
                int cnttick=0; spos=minidx;
                while ((spos<end)&&(content.charAt(spos)=='\'')) {spos++; cnttick++;}

                handleEmphasis(cnttick,res);
                pos=spos;
    	        break;
    	        
    	    case OP_LINK:
                endidx=nextLink+2;
                int linkcnt=2;
                
                while ((endidx<end)&&(linkcnt>0))
                {
                	if ((content.charAt(endidx)=='['))//&&(!((endidx+1<end)&&(content.charAt(endidx-1)!='[')&&(content.charAt(endidx+1)!='h')&&(content.charAt(endidx+1)!='f')&&(content.charAt(endidx+1)!='['))))
                    	linkcnt++;
                	else if ((endidx==end-1)&&(content.charAt(endidx)=='['))
                		linkcnt++; // will be broken anyway
                    else if (content.charAt(endidx)==']')
                    	linkcnt--;
                    endidx++;
                }
                if (linkcnt>0)
                {
                    problem("broken wiki link (pos="+pos+"), ignore.");
                    pos=minidx+2;
                    continue;
                }
                handleLink(content,minidx+2,endidx-2,res);
                pos=endidx;
                break;

    	    case OP_WEBLINK:
                endidx=nextWebLink+1;
                linkcnt=1;
                
                while ((endidx<end)&&(linkcnt>0))
                {
                	// workaround for dangling [ symbols that are not part of links (for example, in interval definitions)
                	if ((content.charAt(endidx)=='[')&&(!((endidx+1<end)&&(content.charAt(endidx-1)!='[')&&(content.charAt(endidx+1)!='h')&&(content.charAt(endidx+1)!='f')&&(content.charAt(endidx+1)!='['))))
                    	linkcnt++;
                	else if ((endidx==end-1)&&(content.charAt(endidx)=='['))
                		linkcnt++; // will be broken anyway
                    else if (content.charAt(endidx)==']')
                    	linkcnt--;
                    endidx++;
                }
                if (linkcnt>0)
                {
                    problem("broken weblink, ignore.");
                    res.append("[");
                    pos=nextWebLink+1;
                    continue;
                }
                handleWebLink(content,minidx+1,endidx-1,res);
                pos=endidx;
                break;

    	    case OP_INDENT:
                // count number of initial : characters 
                int coltick=0; spos=minidx; boolean haveList=false;
                while (spos<end)
                {
                	char c=content.charAt(spos);
                	if (c==':') coltick++;
                	else if (c=='*')
                	{
                		coltick++; haveList=true; break;
                	}
                	else if (c=='#')
                	{
                		coltick++; haveList=true; break;
                	}
                	else if (c==';')
                	{
                		coltick++; haveList=true; break;
                	}
                	else break;
                	spos++;
                }

                if (haveList==false)
                {
	                endidx=content.indexOf('\n', spos);
	                if (endidx==-1)
	                {
	                    handleIndentation(content,coltick,spos,end,res);
	                    pos=end;
	                	break;
	                }
	                else if (endidx<end)
	                {
		                int linkopenidx=content.substring(spos, endidx).lastIndexOf("[[");
		                int linkcloseidx=content.substring(spos, endidx).lastIndexOf("]]");
		                
		                if ((linkopenidx!=-1)&&(linkcloseidx==-1))
		                {
		                	linkcloseidx=content.substring(endidx).indexOf("]]");
		                	
		                	if (linkcloseidx!=-1) endidx=content.indexOf('\n', endidx+linkcloseidx);
		                	else endidx=-1;
		                	
		                	if ((endidx==-1)||(endidx>=end))
		                	{
			                    handleIndentation(content,coltick,spos,end,res);
			                    pos=end;
			                	break;		                		
		                	}
		                }
		                
	                    handleIndentation(content,coltick,spos,endidx+1,res);
	                    pos=endidx+1;
	                	break;
	                }
	                else
	                {
	                	problem("malformed indentation (no following linefeed), ignored.");
	                	pos=spos;
	                	break;
	                }
                }
                // else, we silently pass through to list handling
                
    	    case OP_LIST:
    	    	endidx=minidx+1;
    	    	while (endidx<end)
    	    	{
    	    		int idx=content.indexOf('\n', endidx);
    	    		if (idx==-1)
    	    		{
    	    			// done
    	    			endidx=end;
    	    			break;
    	    		}

    	    		if (idx+1<end)
    	    		{
    	    			char c=content.charAt(idx+1);
    	    			
//    	    			String test=content.substring(idx+1,idx+10);
//    	    			if (c=='\n')
//    	    			{
//    	    				endidx=idx+1; continue;
//    	    			}
    	    			if ((c!='*')&&(c!='#')&&(c!=':')&&(c!=';'))
    	    			{
    	    				endidx=idx;
    	    				break;
    	    			}
    	    			endidx=idx+1;
    	    		}
    	    		else
    	    		{
    	    			endidx=end;
    	    			break;
    	    		}
    	    	}
    	    	
    	    	handleList(content,minidx,endidx,res);
    	    	pos=endidx; // we need the following \n for parsing
    	    	break;
    	    	
    	    case OP_DOUBLELF:
    	    	if ((haveBold)||(haveItalics)) fixOpenEmphasisTags(res);
    	    	if (haveParagraph)
    	    		closeTagLn(res,WikiConstants.paragraphTag);
    	    	openTagLn(res,WikiConstants.paragraphTag);
    	    	haveParagraph=true;
    	    	pos=minidx+1; // we need the final \n in the next step to detect, for example, lists
    	    	break;
	        default:
	            if (minidx==Integer.MAX_VALUE)
	            {
	                res.append(XML.XMLify2(content.substring(pos, end)));
	                pos=-1;
	            }
	            else
	            {
	                pos=minidx+1;
	            }
                break;
    	    }
    	}
    	
    	if ((haveBold)||(haveItalics)) fixOpenEmphasisTags(res);

    	if (haveParagraph) closeTagLn(res,WikiConstants.paragraphTag);
    	
    	haveParagraph=haveParagraphCache;
    	haveBold=haveBoldCache;
    	haveItalics=haveItalicsCache;
    	italicsFirst=italicsFirstCache;
    	
    	boolean fixme=false;
    	for (int i=0;i<openHeaders.length;i++)
    	{
        	if (openHeaders[i])
        	{
        		fixme=true; break;
        	}
    	}
    	
    	if (fixme) cleanupHeaders(res);
    	
    	for (int i=0;i<openHeaders.length;i++)
    	{
        	openHeaders[i]=openHeadersCache[i];
    	}
    }
    
    private final static int OPENLINK=0;
    private final static int CLOSELINK=1;
    private final static int OPENROW=2;
    private final static int DOUBLEHEADER=3;
    private final static int SINGLEHEADER=4;
    private final static int EMPTYCOLCLOSETABLE=5;
    private final static int DOUBLECOL=6;
    private final static int CAPTION=7;
    private final static int CLOSETABLE=8;
    private final static int OPENTABLE=9;
    private final static int NEWLINECOL=10;
    private final static int NEWLINESPACECOL=11;
    private final static int PIPE=12;
    private final static int SINGLESPACEHEADER=13;

    private static final String tabseps[]=
    {"[","]","|-","!!","\n!","||}","||","|+","|}","{|","\n|","\n |","|","\n !"};

    private void handleTable(String content, int startidx, int endidx, StringBuffer res)
    {
        boolean inRow=false;
        
        boolean inLink=false; // reading link description, ignore whatever table commands are seen
        
        int currentLinkCount=0;
        
        String lastcontent="";
        int lastidx=startidx;
        int lastmatch=-1;
        int pipeidx=-1;
        
        while (startidx<endidx)
        {
            int match=-1;
            
            int minidx=Integer.MAX_VALUE;
            
            for (int i=0;i<tabseps.length;i++)
            {
            	if ((inLink)&&(i>1)) break; // ignore whatever we see when we are within a link description
            	
                int idx=content.indexOf(tabseps[i], startidx);
                if ((i==NEWLINECOL)&&(idx!=-1)) idx++;
                else if ((i==NEWLINESPACECOL)&&(idx!=-1)) idx+=2;
                else if ((i==SINGLEHEADER)&&(idx!=-1)) idx++;
                else if ((i==SINGLESPACEHEADER)&&(idx!=-1)) idx+=2;
                
                if ((idx!=-1)&&(idx<=endidx)&&(idx<minidx))
                {
	                    match=i; minidx=idx;
                }
            }
            
            if (match==-1)
            {
                //hm
                problem("no match found");
                break;
            }
            
            if (match==OPENLINK)
            {
//                if ((inLink==false)&&(lastcontent.length()>0))
//                {
//                	if (pipeidx==-1)
//               		{
//                		pipeidx=lastcontent.length();
//                    	lastcontent+="|";
//               		}
//                }
            	// workaround for dangling [ symbols that are not part of links (for example, in interval definitions)
            	
            	if (!((minidx+1<endidx)&&(content.charAt(minidx-1)!='[')&&(content.charAt(minidx+1)!='h')&&(content.charAt(minidx+1)!='f')&&(content.charAt(minidx+1)!='[')))
            	{
            		currentLinkCount++;
                	inLink=true;
            	}
                lastcontent+=content.substring(lastidx, minidx+1);//.trim();
                startidx=minidx+1;
                lastidx=minidx+1;
                continue;
            }
            else if (match==CLOSELINK)
            {
            	currentLinkCount--;
            	if (currentLinkCount<0)
            		currentLinkCount=0;
            	if (currentLinkCount==0) inLink=false;
                lastcontent+=content.substring(lastidx, minidx+1);
//                lastcontent=lastcontent.trim();
                startidx=minidx+1;
                lastidx=minidx+1;
                continue;
            }
            else if (match==OPENTABLE)
            {
            	int tablecnt=1;
            	int idx=minidx+2;
            	while (idx<endidx)
            	{
            		boolean start=false;
            		int fidx=Integer.MAX_VALUE;
            		
            		int sidx=content.indexOf("{|", idx);
            		if ((sidx!=-1)&&(sidx<fidx))
            		{
            			fidx=sidx; start=true;
            		}
            		int eidx=content.indexOf("|}", idx);
            		if ((eidx!=-1)&&(eidx<fidx))
            		{
            			fidx=eidx; start=false;
            		}
            		if (fidx<Integer.MAX_VALUE)
            		{
            			if (start) tablecnt++;
            			else tablecnt--;
                		idx=fidx+2;
                		if (tablecnt==0) break;
            		}
            		else break;
            	}
            	lastcontent+=content.substring(minidx, idx);
            	minidx=idx-2;
            	startidx=idx;
            	lastidx=idx;
            	continue;

            }
            else if (match==PIPE)
            {
//                if (lastcontent.length()>0)
//                {
//                	if (pipeidx==-1) pipeidx=lastcontent.length();
//                	lastcontent+="|";
//                }
                String sub=content.substring(lastidx, minidx);//.trim();
                if (sub.length()>0)
                {
                    lastcontent+=sub;
                	if (pipeidx==-1) pipeidx=lastcontent.length();
                	lastcontent+="|";                	
                }
                else if (lastcontent.endsWith("|")==false)
                	lastcontent+="|";                	

                startidx=minidx+1;
                lastidx=minidx+1;
                continue;
            }
            else
            {
//                if (lastcontent.length()>0) lastcontent+="|";
                lastcontent+=content.substring(lastidx, minidx).trim();
//                if (lastcontent.endsWith("|"))
//                {
//                	lastcontent=lastcontent.substring(0,lastcontent.length()-1).trim();
//                	pipeidx=lastcontent.indexOf('|');
//                }

                switch(lastmatch)
                {
                    case -1:
                    // table has just started
                        if (lastcontent.length()>0)
                        {
                        	int cridx=lastcontent.indexOf('\n');
                        	if (cridx!=-1)
                        	{
                        		String attrs=lastcontent.substring(0, cridx);
                        		String c=lastcontent.substring(cridx+1);
                                openTagLn(res,WikiConstants.tableTag,attrs);
                        		if (c.trim().length()>0)
                        		{
                        			openTagLn(res,WikiConstants.tableRowTag);
                        			openTagLn(res,WikiConstants.tableCellTag);
                        			handleContent(c,res);
                        			closeTagLn(res,WikiConstants.tableCellTag);
                        			closeTagLn(res,WikiConstants.tableRowTag);
                        		}
                        	}
                        	else openTagLn(res,WikiConstants.tableTag,lastcontent);
                        }
                        else
                            openTagLn(res,WikiConstants.tableTag);                                                    	
                        break;
                    case OPENROW:
//                        System.out.println("begin row");
                        if (match!=CLOSETABLE)
                        {
//                            int pidx=lastcontent.indexOf('|'); int lidx=lastcontent.indexOf("[[");
//                            if ((pidx!=-1)&&((lidx==-1)||(pidx<lidx)))
                        	if (pipeidx!=-1)
                        	{
                            	openTagLn(res,WikiConstants.tableRowTag,lastcontent.substring(0,pipeidx));
                                handleContent(lastcontent,pipeidx+1,lastcontent.length(),res);
                            }
                            else if (lastcontent.length()>0)
                                openTagLn(res,WikiConstants.tableRowTag,lastcontent);
                            else
                                openTagLn(res,WikiConstants.tableRowTag);
                            inRow=true;
                        }
                        else inRow=false;
                        break;
                    case DOUBLECOL:
                    case NEWLINECOL:
                    case NEWLINESPACECOL:
//                        System.out.println("begin column: "+lastcontent);
                    	if (pipeidx!=-1)
                        {
                        	if (inRow==false)
                        	{
                        		openTagLn(res,WikiConstants.tableRowTag);
                        		inRow=true;
                        	}
                            openTag(res,WikiConstants.tableCellTag,lastcontent.substring(0,pipeidx));
                            handleContent(lastcontent,pipeidx+1,lastcontent.length(),res);
                            closeTagLn(res,WikiConstants.tableCellTag);
                        }
                        else
                        {
                        	if (inRow==false)
                        	{
                        		openTagLn(res,WikiConstants.tableRowTag);
                        		inRow=true;
                        	}
                            openTag(res,WikiConstants.tableCellTag);
                            handleContent(lastcontent,0,lastcontent.length(),res);
                            closeTagLn(res,WikiConstants.tableCellTag);
                        }
                        
                        break;
                    case CAPTION:
//                        System.out.println("caption");
                    	if (pipeidx!=-1)
                        {
                            openTag(res,WikiConstants.tableCaptionTag,lastcontent.substring(0,pipeidx));
                            handleContent(lastcontent,pipeidx+1,lastcontent.length(),res);
                            closeTagLn(res,WikiConstants.tableCaptionTag);
                        }
                        else
                        {
                            openTag(res,WikiConstants.tableCaptionTag);
                            handleContent(lastcontent,0,lastcontent.length(),res);
                            closeTagLn(res,WikiConstants.tableCaptionTag);
                        }
                        break;
                    case DOUBLEHEADER:
                    case SINGLEHEADER:
                    case SINGLESPACEHEADER:
//                        System.out.println("begin header cell: "+lastcontent);
                    	if (pipeidx!=-1)
                        {
                            openTag(res,WikiConstants.tableHeaderTag,lastcontent.substring(0,pipeidx));
                            handleContent(lastcontent,pipeidx+1,lastcontent.length(),res);
                            closeTagLn(res,WikiConstants.tableHeaderTag);
                        }
                        else
                        {
                            openTag(res,WikiConstants.tableHeaderTag);
                            handleContent(lastcontent,0,lastcontent.length(),res);
                            closeTagLn(res,WikiConstants.tableHeaderTag);
                        }
                        
                        break;
                    case OPENTABLE:
                    	break;
                    default:
                        problem("no match: "+lastcontent);
                        break;
                }

                lastcontent=""; pipeidx=-1;
            }
            
            switch(match)
            {
                case OPENROW: // row separator
                    if (inRow)
                    {
//                        System.out.println("end row");
                        closeTagLn(res,WikiConstants.tableRowTag);
                    }
                    inRow=true;
                    break;
                case CAPTION: // caption
                    if (inRow)
                    {
//                        System.out.println("end row");
                        closeTag(res,WikiConstants.tableRowTag);
                    }
                    inRow=false;
                    break;
                case CLOSETABLE: // end of table
                case EMPTYCOLCLOSETABLE:
                    if (inRow)
                    {
//                        System.out.println("end row");
                        closeTagLn(res,WikiConstants.tableRowTag);
                    }
                    closeTagLn(res,WikiConstants.tableTag);
                    
//                    System.out.println("end of table");
                    break;
                case OPENTABLE: // embedded table definition
                	int tablecnt=1;
                	int idx=minidx+2;
                	while (idx<endidx)
                	{
                		boolean start=false;
                		int fidx=Integer.MAX_VALUE;
                		
                		int sidx=content.indexOf("{|", idx);
                		if ((sidx!=-1)&&(sidx<fidx))
                		{
                			fidx=sidx; start=true;
                		}
                		int eidx=content.indexOf("|}", idx);
                		if ((eidx!=-1)&&(eidx<fidx))
                		{
                			fidx=eidx; start=false;
                		}
                		if (fidx<Integer.MAX_VALUE)
                		{
                			if (start) tablecnt++;
                			else tablecnt--;
                    		idx=fidx+2;
                    		if (tablecnt==0) break;
                		}
                		else break;
                	}
                	handleContent(content,minidx,idx,res);
                	minidx=idx-2;
                	break;
                case SINGLEHEADER:
                case DOUBLEHEADER: // header column separator
                case DOUBLECOL:
                case NEWLINECOL: 
                case NEWLINESPACECOL: // column separator
                case SINGLESPACEHEADER:
                	break;
                default:
                    problem("unexpected match "+match);
                    break;
            }
            lastmatch=match;
            startidx=minidx+tabseps[match].length();
            if (match==NEWLINECOL) startidx--;
            else if ((match==NEWLINESPACECOL)) startidx-=2;
            else if (match==SINGLEHEADER) startidx--;
            else if (match==SINGLESPACEHEADER) startidx-=2;
            lastidx=startidx;
        }
        
    }
    
    private static final String headerTags[]=
    {
        WikiConstants.sectionTag, WikiConstants.subsec1Tag, WikiConstants.subsec2Tag, WikiConstants.subsec3Tag, WikiConstants.subsec4Tag, WikiConstants.subsec5Tag
    };
    
    private final boolean openHeaders[]=new boolean[headerTags.length];
    
    private void handleHeader(String content, int start, int end, int cnt, StringBuffer res)
    {
    	if ((haveBold)||(haveItalics)) fixOpenEmphasisTags(res);
    	
    	if (haveParagraph)
    		closeTagLn(res,WikiConstants.paragraphTag);
    	
        cnt=cnt-2; // 1 -> section, 2-> subsection, etc
        
        for (int i=openHeaders.length-1;i>=cnt;i--)
        	if (openHeaders[i])
        	{
        		closeTagLn(res,headerTags[i]);
        		openHeaders[i]=false;
        	}

        openTagLn(res,headerTags[cnt]);
        openHeaders[cnt]=true;
        
        openTag(res,WikiConstants.sectionTitleTag);
        handleContent(content,start,end,res);
        closeTagLn(res,WikiConstants.sectionTitleTag);
        
        openTagLn(res,WikiConstants.paragraphTag);
        haveParagraph=true;
    }

    private boolean haveItalics=false;
    private boolean haveBold=false;
    private boolean italicsFirst=false;
    
    private void handleEmphasis(int cnt, StringBuffer res)
    {
    	boolean emitApos=false;
    	
    	if (cnt==4)
    	{
    		if (haveItalics==false)
    		{
    			// heuristics: if we are not yet in italics mode, this is probably
    			// a bold and a following apos
    			cnt=3; emitApos=true;
    		}
    		// else, somebody probably disabled and reenabled italics together
    		// so we ignore it
    	}
    	if (cnt==4)
    	{
    		// ignore.
    	}
    	else if (cnt==2)
        {
        	if ((haveItalics)&&(haveBold))
        	{
        		// disable both, then reenable bold
        		
        		if (italicsFirst)
        		{
        			closeTag(res,WikiConstants.boldTag);
        			closeTag(res,WikiConstants.italicsTag);
        		}
        		else
        		{
        			closeTag(res,WikiConstants.italicsTag);        			
        			closeTag(res,WikiConstants.boldTag);
        		}
        		italicsFirst=false;
        		haveItalics=false;
                openTag(res,WikiConstants.boldTag);
        	}
        	else if (haveItalics)
            {
                closeTag(res,WikiConstants.italicsTag);
                haveItalics=false;
            }
            else
            {
                openTag(res,WikiConstants.italicsTag);
                haveItalics=true;
                if (haveBold) italicsFirst=false;
                else italicsFirst=true;
            }
        }
        else if ((cnt==3)||(cnt==6)) // some pages do strange things...
        {
        	if ((haveItalics)&&(haveBold))
        	{
        		// disable both, then reenable bold
        		
        		if (italicsFirst)
        		{
        			closeTag(res,WikiConstants.boldTag);
        			closeTag(res,WikiConstants.italicsTag);
        		}
        		else
        		{
        			closeTag(res,WikiConstants.italicsTag);        			
        			closeTag(res,WikiConstants.boldTag);
        		}
        		italicsFirst=true;
        		haveBold=false;
                openTag(res,WikiConstants.italicsTag);
        	}
        	else if (haveBold)
            {
                closeTag(res,WikiConstants.boldTag);
                haveBold=false;
            }
            else
            {
            	if (emitApos)
            	{
	                openTag(res,WikiConstants.boldTag);
	                haveBold=true;
	                if (haveItalics) italicsFirst=true;
	                else italicsFirst=false;
            	}
            	else if (haveItalics) // heuristics: if italics is already enabled, ''' usually means switch it off + apos
            	{
	                closeTag(res,WikiConstants.italicsTag);
	                haveItalics=false;
	                italicsFirst=false;
	                emitApos=true;
            	}
            	else
            	{
	                openTag(res,WikiConstants.boldTag);
	                haveBold=true;
	                if (haveItalics) italicsFirst=true;
	                else italicsFirst=false;
            	}
            }            
        }
        else if (cnt==5)
        {
            if ((haveBold)&&(haveItalics))
            {
                if (italicsFirst)
                {
                    closeTag(res,WikiConstants.boldTag);
                    closeTag(res,WikiConstants.italicsTag);
                }
                else
                {
                    closeTag(res,WikiConstants.italicsTag);
                    closeTag(res,WikiConstants.boldTag);
                }
                haveBold=false;
                haveItalics=false;
                italicsFirst=false;
            }
            else if ((haveBold==false)&&(haveItalics==false))
            {
                openTag(res,WikiConstants.boldTag);
                haveBold=true;
                openTag(res,WikiConstants.italicsTag);
                haveItalics=true;
                italicsFirst=false;
            }
            else if (haveBold)
            {
                closeTag(res,WikiConstants.boldTag);
                haveBold=false;
                openTag(res,WikiConstants.italicsTag);
                haveItalics=true;
                italicsFirst=true;
            }
            else if (haveItalics)
            {
                closeTag(res,WikiConstants.italicsTag);
                haveItalics=false;
                openTag(res,WikiConstants.boldTag);
                haveBold=true;
                italicsFirst=false;
            }
        }
        else
        {
            problem("unexpected emphasis count: "+cnt);
        }
        
        if (emitApos) res.append("&apos;");
    }
    
    private void handleLink(String content, int start, int end, StringBuffer res)
    {
        //check if we're dealing with special namespaces
        
    	// such a link just adds a link to the category page, but does not add the page to the category.
    	String lc_content = content.substring(start).toLowerCase();
    	int category_len = 0;
    	if ((lc_content.startsWith("：category:") && (category_len = "：category:".length()) > 0) 
    			|| (lc_content.startsWith("：模板:") && (category_len = "：模板:".length()) > 0))
    	{
    		if (start + category_len < end) 
    			handleContent(content.substring(start + category_len, end), res);
    		return;
    	}
    	
    	if (content.charAt(start)==':') 
    		start++; // ignore any leading : (which is a shortcut for the current article namespace)
    	
        if (lc_content.startsWith("image:") 
        		|| lc_content.startsWith("文件:")
        		|| lc_content.startsWith("档案:")
        		|| lc_content.startsWith("图像:"))
            handleImageLink(content,start,end,res);
        else if (lc_content.startsWith("category:")
        		|| lc_content.startsWith("模板:"))
            handleCategory(content,start,end,res);
        else if (((content.length()>=start+"portal:".length())&&(content.substring(start,start+"portal:".length()).toLowerCase().startsWith("portal:")))||(content.indexOf(':',start)==-1)||(content.indexOf(':', start)>=end))
        {
            String target; // link target
            String text; // link text (shown on the page)
            int idx=content.indexOf('|',start);
            if ((idx==-1)||(idx>=end))
            {
                target=content.substring(start,end);
                text=target;
            }
            else
            {
                target=content.substring(start, idx);                
                text=content.substring(idx+1, end);
            }
            // create link
            
            boolean validLink=true;

        	String prefix=null;
        	int colidx=content.indexOf(':', start);
        	if ((colidx>-1)&&(colidx<end))
        	{
        		prefix=content.substring(start,colidx).toLowerCase();
        		
        		// at this time, we ignore all links to different namespaces
        		if ((prefix.compareTo("wikipedia")==0)||(prefix.compareTo("template")==0))
        		{
        			res.append(" "); handleContent(text,res); res.append(" ");
        		}
        		validLink=false;
        	}
        	else 
        		validLink=true;
        	
            if (validLink)
            {
                String anchor=null;
                
                int pos=target.indexOf('#');
                if (pos>-1)
                {
                    // if the '#' actually belongs to an XML entity, leave it as it is;
                    // otherwise (when it is part of the pointer) remove it
                    if ((pos==0)||(target.charAt(pos-1)!='&'))
                    {
                        if (target.length()>pos) anchor=target.substring(pos+1);
                        target=target.substring(0,pos);
                    }
                }
                
                String link=target;
                
                if (link.length()>1)
                {
                    link=Character.toUpperCase(link.charAt(0))+link.substring(1);
                }
                else if (link.length()==1)
                    link=Character.toString(Character.toUpperCase(link.charAt(0)));

                int i=0;
                while (redirections.containsKey(link))
                {
                    i++;
                    if (i>20) 
                    	break;
                    link=redirections.get(link);
                }
                
                boolean linkExists=((link.length()==0)||(articles.containsKey(link)));
                
                String url=makeWikiURL(useArticleIDs?articles.get(link):link);

                if (anchor!=null)
                {
                	anchor=anchor.replaceAll("\"", "");
                	if ((anchor.startsWith("CITEREF"))||(anchor.startsWith("endnote")))
                	{	
                		anchor=anchor.replaceAll(" ", "_");
                		url+="#xpointer(//cite[@id=%22"+UTF82ASCII.encode(anchor)+"%22])";
                	}
                	else if (anchor.startsWith("ref_")) // backward reference, unsupported
                	{
                		handleContent(text,res);
                		return;
                	}
                	else
                		url+="#xpointer(//*[./st=%22"+UTF82ASCII.encode(anchor)+"%22])";
                }

                // the manual claims that following text in parenthesis is automatically dropped
                // (from the text, not from the link), but that is actually wrong.
                
//                pos=text.indexOf('(');
//                if (pos!=-1) text=text.substring(0, pos).trim();
                
//              if (linkExists==false)
//              {
//                  System.out.println("link target "+link+" ["+url+"] does not exist!");
//              }
                
                // check if the link target has an annotation in the database
                
                List<label> labelset=new LinkedList<label>();

                if ((createAnnotations)&&(phase==1))
                {
                    labelset=new LinkedList<label>(readAndConsolidateLabels(target));
                    
                }

                if (createWikiLinks)
                {
                    if (linkExists)
                    {
                        openAnnotationTags(labelset,res,false);
                        
                        openTag(res,WikiConstants.wikilinkTag," xlink:href=\""+url+"\" xlink:type=\"simple\"");
                        handleContent(text,res);
//                        res.append(XML.XMLify2(text));
                        closeTag(res,WikiConstants.wikilinkTag);

                        closeAnnotationTags(labelset,res);
                    }
                    else if (ignoreDeadLinks)
                    {
                        handleContent(text,res);
//                      res.append(XML.XMLify2(text));
                    }
                    else if (markDeadLinks)
                    {
                        openAnnotationTags(labelset,res,false);
                        
                        openTag(res,WikiConstants.deadlinkTag);
                        handleContent(text,res);
//                      res.append(XML.XMLify2(text));
                        closeTag(res,WikiConstants.deadlinkTag);

                        closeAnnotationTags(labelset,res);
                    }
                    else
                    {
                        openAnnotationTags(labelset,res,false);
                        
                        openTag(res,WikiConstants.wikilinkTag);
                        handleContent(text,res);
//                      res.append(XML.XMLify2(text));
                        closeTag(res,WikiConstants.wikilinkTag);

                        closeAnnotationTags(labelset,res);
                   }
                }
                else
                {
                    openAnnotationTags(labelset,res,false);
                    
                    openTag(res,WikiConstants.wikilinkTag);
                    handleContent(text,res);
//                  res.append(XML.XMLify2(text));
                    closeTag(res,WikiConstants.wikilinkTag);

                    closeAnnotationTags(labelset,res);
                }
            }
            else
            {
                handleContent(text,res);
//              res.append(XML.XMLify2(text));
            }
        }else if(content.substring(start, end).indexOf(":") != -1){
        	int titlePos = content.indexOf(":", start);
        	String title = content.substring(titlePos + 1,end);
        	while(title.indexOf(":") != -1){      	
        		title = title.substring(title.indexOf(":") + 1);
        	}
        	if(articles.containsKey(title)){
        		String link = articles.get(title);
        		
        		String url=makeWikiURL(link);
        		openTag(res,WikiConstants.wikilinkTag," xlink:href=\""+url+"\" xlink:type=\"simple\"");
        		closeTag(res,WikiConstants.wikilinkTag);
        	}
        }
    }
    
    private final static HashSet<String> imageTypes;
    private final static HashSet<String> imageLocations;
    static
    {
    	imageTypes=new HashSet<String>();
    	imageTypes.add("thumb");
    	imageTypes.add("thumbnail");
    	imageTypes.add("border");
    	imageTypes.add("frame");
    	imageLocations=new HashSet<String>();
    	imageLocations.add("right");
    	imageLocations.add("left");
    	imageLocations.add("none");
    	imageLocations.add("center");	
    }
    
    private void handleImageLink(String content, int start, int end, StringBuffer res)
    {
        String name; // link target
        String text=null; // link text (shown on the page)
        
        String type=null;
        String location=null;
        String upright=null;
        String size=null;
        
        
        int idx=content.indexOf('|',start);
        if ((idx==-1)||(idx>=end))
        {
            name=content.substring(start,end);
            idx=name.indexOf(':');
            if (idx!=-1) name=name.substring(idx+1);
            text=name;
        }
        else
        {
            name=content.substring(start,idx);
            String params=content.substring(idx+1, end);
            idx=name.indexOf(':');
            if (idx!=-1) name=name.substring(idx+1);
            
            int linkpos=params.indexOf("[[");
            int pend=params.length();
            if (linkpos!=-1) pend=linkpos;
            
            int pos=0;
            while ((pos>=0)&&(pos<pend))
            {
            	idx=params.indexOf('|',pos);
            	String txt;
            	if ((idx==-1)||(idx>pend))
            	{
            		txt=params.substring(pos);
            	}
            	else txt=params.substring(pos, idx);
            	if (imageTypes.contains(txt)) type=txt;
            	else if (imageLocations.contains(txt)) location=txt;
            	else if (txt.startsWith("upright")) upright=txt;
            	else if (txt.endsWith("px")) size=txt;
            	else text=txt;
            	if (idx==-1) break;
            	pos=idx+1;
            	continue;
            }
        }
	
		if (createImageLinks)
		{
			String attributes="src=\""+XML.XMLify2(makeImageURL(name.trim()).replaceAll("\"", "%20"))+"\"";
			if (type!=null) attributes+=" type=\""+type+"\"";
			if (location!=null) attributes+=" location=\""+location+"\"";
			if (upright!=null) attributes+=" upright=\""+upright+"\"";
			if (size!=null) attributes+=" width=\""+size+"\"";
			else attributes+=" width=\"150px\"";
			
			if (attributes.length()>0) openTagLn(res,WikiConstants.imageTag,attributes);
			else openTagLn(res,WikiConstants.imageTag);
			
			if (createHTMLImage)
			{
				emptyTagLn(res,"img","xmlns=\"http://www.w3.org/1999/xhtml\" src=\""+XML.XMLify2(makeImageURL(name.trim()).replaceAll("\"", "%20"))+"\" "+(size==null?"height=\"150pt\"":"width=\""+size+"\""));
			}
//			emptyTagLn(res,WikiConstants.imageContentTag,"xlink:type=\"simple\" xlink:href=\""+XML.XMLify2(makeImageURL(name.trim()))+"\" "+(size==null?"height=\"150pt\"":"width=\""+size+"\""));
			if (text!=null)
			{
				openTagLn(res,WikiConstants.imageCaptionTag);
				handleContent(text,res); res.append("\n");
				closeTagLn(res,WikiConstants.imageCaptionTag);
			}
			
			closeTagLn(res,WikiConstants.imageTag);
		}
		else
		{
			if (text!=null)
			{
				openTagLn(res,WikiConstants.imageTag);
				handleContent(text,res); res.append("\n");
				closeTagLn(res,WikiConstants.imageTag);
			}
			else emptyTagLn(res,WikiConstants.imageTag);
		}

    }
    
    private void handleCategory(String content, int start, int end, StringBuffer res)
    {
    	start=start+"category:".length();
        String target; // link target
        int idx=content.indexOf('|',start);
        if ((idx==-1)||(idx>end))
        {
            target=content.substring(start,end);
        }
        else
        {
            target=content.substring(start, idx);
        }
        categories.add(target);
    }

    private void handleIndentation(String content,int coltick,int start, int end, StringBuffer res)
    {
    	openTagLn(res,WikiConstants.indentTag,"level=\""+coltick+"\"");
    	handleContent(content,start,end,res);
    	closeTagLn(res,WikiConstants.indentTag);
    }

    private void cleanupHeaders(StringBuffer res)
    {
        for (int i=openHeaders.length-1;i>=0;i--)
        	if (openHeaders[i])
        	{
        		closeTagLn(res,headerTags[i]);
        		openHeaders[i]=false;
        	}
    }
    
    private  void fixOpenEmphasisTags(StringBuffer res)
    {
        if ((haveBold)&&(haveItalics))
        {
            if (italicsFirst)
            {
                closeTag(res,WikiConstants.boldTag);
                closeTag(res,WikiConstants.italicsTag);
            }
            else
            {
                closeTag(res,WikiConstants.italicsTag);
                closeTag(res,WikiConstants.boldTag);
            }
            haveBold=false;
            haveItalics=false;
        }
        else if (haveBold)
        {
            closeTag(res,WikiConstants.boldTag);
            haveBold=false;
            italicsFirst=false;
        }
        else if (haveItalics)
        {
            closeTag(res,WikiConstants.italicsTag);
            haveItalics=false;
            italicsFirst=false;
        }

    }
    
    private void handleWebLink(String content, int start, int end, StringBuffer res)
    {
    	if (start>=end) return;
    	
    	
    	String prefix=content.substring(start,Math.min(end, start+10));
    	
    	if ((prefix.startsWith("http://")==false)&&(prefix.startsWith("ftp://")==false))
    	{
    		// this is not really a link, probably someone just wanted to use square brackets
    		
    		res.append("[").append(XML.XMLify2(content.substring(start, end))).append("]");
    		return;
    	}
    	
        String target; // link target
        String text; // link text (shown on the page)
        int idx=content.indexOf(' ',start);
        if ((idx==-1)||(idx>=end))
        {
            target=content.substring(start,end);
            text=target;
        }
        else
        {
            target=content.substring(start, idx);
            text=content.substring(idx+1, end);
        }
        
        // create link
        
        if (createWebLinks)
        {
                openTag(res,WikiConstants.weblinkTag," xlink:href=\""+target+"\" xlink:type=\"simple\"");
                handleContent(text,res);
                closeTag(res,WikiConstants.weblinkTag);
        }
        else
        {
            openTag(res,WikiConstants.weblinkTag);
            handleContent(text,res);
            closeTag(res,WikiConstants.weblinkTag);
        }
    }

    private void handleList(String content, int start, int end, StringBuffer res)
    {
//    	System.out.println(content.substring(start, end));
    	
    	String lines[]=content.substring(start, end).split("\n");
    	
    	openTagLn(res,WikiConstants.listTag);
    	
    	for (int i=0;i<lines.length;i++)
    	{
//    		System.out.println(lines[i]);
    		int cnt=0;
    		
    		while (cnt<lines[i].length())
    		{
    			char c=lines[i].charAt(cnt);
    			if ((c!='*')&&(c!='#')&&(c!=':')&&(c!=';'))
    				break;
    			else cnt++;
    		}
    		
    		String type;
    		if (lines[i].charAt(cnt-1)=='*') type="bullet";
    		else if (lines[i].charAt(cnt-1)=='#') type="number";
    		else if (lines[i].charAt(cnt-1)==';') type="definition";
    		else if (lines[i].charAt(cnt-1)==':') type="indent";
    		else type="undef";
    		
    		openTagLn(res,WikiConstants.listEntryTag,"type=\""+type+"\" level=\""+cnt+"\"");    		
    		handleContent(lines[i],cnt,lines[i].length(),res);
    		closeTagLn(res,WikiConstants.listEntryTag);
    	}
    	
    	closeTagLn(res,WikiConstants.listTag);
    }
    
    private HashMap<Integer,String> references; // stores text for references, identified by unique numbers
    
    private HashMap<String,Integer> refnames; // stores name-to-id mapping for labeled references
    private HashMap<Integer,String> refgroups; // stores id-to-group mappings for labeled references
    int lastreference;
    
    private HashMap<String,String> nowikis;
    int lastnowiki;

    private HashMap<String,String> maths;
    int lastmath;

    private HashMap<String,String> imagemaps;
    int lastimagemap;
    
    private HashMap<String,String> cites;
    int lastcite;
    
    private final static String CLIX_REFERENCE="__CLIX_REFERENCE_";
    private final static String CLIX_REFERENCE_LIST="__CLIX_REFERENCELIST_";
    private final static String CLIX_NOWIKI="__CLIX_NOWIKI_";
    private final static String CLIX_MATH="__CLIX_MATH_";
    
    private final static String CLIX_IMAGEMAP="__CLIX_IMAGEMAP_";
    
    private final static String CLIX_CITE="__CLIX_CITE_";
    private final static String CLIX_ENDCITE="__CLIX_ENDCITE_";
    
    private String fixHTMLTags(String content, boolean reset)
    {
    	StringBuffer res=new StringBuffer(content.length());
    	
    	if (reset)
    	{
	    	references=new HashMap<Integer,String>();
	    	refgroups=new HashMap<Integer,String>();
	    	refnames=new HashMap<String,Integer>();
	    	lastreference=0;
	    	imagemaps=new HashMap<String,String>();
	    	lastimagemap=0;
	    	cites=new HashMap<String,String>();
	    	lastcite=0;
    	}
    	
    	int pos=0;
    	
    	boolean cellopen=false;
    	
    	while ((pos>-1)&&(pos<content.length()))
    	{
    		int idx=content.indexOf('<', pos);
    		
    		if (idx==-1)
    		{
    			res.append(content.subSequence(pos, content.length()));
    			break;
    		}
    		
    		res.append(content.subSequence(pos, idx));
    		
    		if ((idx+1<content.length())&&(content.charAt(idx+1)!='/')&&(Character.isLetter(content.charAt(idx+1))==false))
    		{
//    			res.append(content.subSequence(idx, idx+1));
    			pos=idx+1;
    			continue;
    		}

    		int endidx=Integer.MAX_VALUE;
    		int type=-1;
    		
    		// index of the next closing bracket
    		int bidx=content.indexOf('>', idx);
    		if ((bidx!=-1)&&(bidx<endidx))
    		{
    			endidx=bidx; type=0;
    		}
    		// index of the next end of an empty tag
    		int fidx=content.indexOf("/>", idx);
    		if ((fidx!=-1)&&(fidx<endidx))
    		{
    			endidx=fidx;
    			type=1;
    		}
    		// index of the next space
    		int sidx=content.indexOf(' ',idx);
    		if ((sidx!=-1)&&(sidx<endidx))
    		{
    			endidx=sidx;
    			type=2;
    		}
    		if (endidx==Integer.MAX_VALUE)
    		{
    			// we silently assume that this was a bogus tag opener and ignore it
    			problem("cannot find end of tag");
//    			res.append(content.subSequence(idx, content.length()));
    			pos=idx+1;
    			continue;
    		}

    		String tag=content.substring(idx+1, endidx);
    		
    		if (tag.compareTo("ref")==0)
    		{
    			String name=null;
    			String group=null;
    			
    			if (type==2)
    			{
    				int nidx=content.indexOf("name=\"", sidx);
    				if ((nidx!=-1)&&(nidx<bidx))
    				{
    					int qidx=content.indexOf('\"', nidx+"name=\"".length());
    					if ((qidx!=-1)&&(qidx<bidx))
    					{
    						name=content.substring(nidx+"name=\"".length(), qidx);
       						if (name.endsWith("\"")) name=name.substring(0,name.length()-1);
       						if (name.startsWith("\"")) name=name.substring(1);
       						if (name.indexOf('<')!=-1) name="";
       						if (name.indexOf('>')!=-1) name="";
    					}
    				}
    				else
    				{
        				nidx=content.indexOf("name = \"", sidx); // used for some automatically generated <ref> tags
        				if ((nidx!=-1)&&(nidx<bidx))
        				{
        					int qidx=content.indexOf('\"', nidx+"name = \"".length());
        					if ((qidx!=-1)&&(qidx<bidx))
        					{
        						name=content.substring(nidx+"name = \"".length(), qidx);
           						if (name.endsWith("\"")) name=name.substring(0,name.length()-1);
           						if (name.startsWith("\"")) name=name.substring(1);
           						if (name.indexOf('<')!=-1) name="";
           						if (name.indexOf('>')!=-1) name="";
        					}
        				}
    					else
        				{
            				nidx=content.indexOf("name=", sidx);
            				if ((nidx!=-1)&&(nidx<bidx))
            				{
            					int qidx=content.indexOf(' ', nidx+1);
            					if ((qidx==-1)||(qidx>bidx)) qidx=bidx;
            					if ((fidx!=-1)&&(fidx<bidx)&&(qidx>fidx)) qidx=fidx;
           						name=content.substring(nidx+5, qidx);
           						if (name.endsWith("\"")) name=name.substring(0,name.length()-1);
           						if (name.startsWith("\"")) name=name.substring(1);
           						if (name.indexOf('<')!=-1) name="";
           						if (name.indexOf('>')!=-1) name="";
            				}    					
        				}    						
    				}
    				nidx=content.indexOf("group=\"", sidx);
    				if ((nidx!=-1)&&(nidx<bidx))
    				{
    					int qidx=content.indexOf('\"', nidx+7);
    					if ((qidx!=-1)&&(qidx<bidx))
    						group=content.substring(nidx+7, qidx);
    				}
    			}
//    			if (name==null)
//    				name=Integer.toString(++lastreference);
    			
    			if (fidx==bidx-1)
    			{
//    				System.out.println("found re-reference: "+name);
    				Integer id=refnames.get(name);
    				int myid=(id==null?0:id);
    				// fix: ignore any re-references to nonexisting references (probably not exactly conforming to the Wikipedia semantics, but good enough for now)
    				if (myid!=0) res.append(CLIX_REFERENCE).append(myid).append("_").append(group==null?"":group).append("_");
    				pos=bidx+1;
    				continue;
    			}
    			else
    			{
    				int refendidx=content.indexOf("</ref>", bidx);
        			if (refendidx!=-1)
        			{
        				int test=content.indexOf("<ref", bidx);
        				if ((test!=-1)&&(test<refendidx))
        				{
        					// there is an embedded reference here, so skip to next </ref>
        					refendidx=content.indexOf("</ref>", test);
        					if (refendidx==-1)
        					{
        						problem("malformed nested reference, skip.");
        						pos=bidx+1; continue;
        					}
        				}
        				String result=fixHTMLTags(content.substring(bidx+1,refendidx),false);
//        				System.out.println("found reference: "+name+" "+result);
        				int myid=0;
        				if (name!=null)
        				{
            				Integer id=refnames.get(name);
            				myid=(id==null?0:id);
        				}
        				if (myid==0)
        				{
        					myid=++lastreference;
        					if (name!=null) refnames.put(name, myid);
        				}
        				references.put(myid, result);
        				if (group!=null) refgroups.put(myid, group);
        				res.append(CLIX_REFERENCE).append(myid).append("_").append(group==null?"":group).append("_");
        				pos=refendidx+"</ref>".length();
        				continue;
        			}
    			}
    			pos=bidx+1;
    			continue;
    		}
    		else if (tag.compareTo("references")==0)
    		{
    			String group=null;
    			if (type==2)
    			{
    				int nidx=content.indexOf("group=\"", sidx);
    				if ((nidx!=-1)&&(nidx<bidx))
    				{
    					int qidx=content.indexOf('\"', nidx+7);
    					if ((qidx!=-1)&&(qidx<bidx))
    						group=content.substring(nidx+7, qidx);
    				}
    				
    			}
    			res.append(CLIX_REFERENCE_LIST).append(group==null?"":group).append("_");
    		}
    		else if (tag.compareTo("nowiki")==0)
    		{
    			if (bidx==-1)
    			{
    				problem("<nowiki> must not have attributes and must not be empty");
    				pos=endidx+1;
    				continue;
    			}
    			int endnowiki=content.indexOf("</nowiki>", bidx);
    			if (endnowiki==-1)
    			{
    				problem("</nowiki> not found.");
    				pos=bidx+1;
    				continue;
    			}
    			if (bidx+1<endnowiki)
    			{
    				nowikis.put(Integer.toString(++lastnowiki), content.substring(bidx+1,endnowiki));
    				res.append(CLIX_NOWIKI).append(lastnowiki).append("_");
    				pos=endnowiki+"</nowiki>".length();
    				continue;
    			}
    		}
    		else if (tag.compareTo("imagemap")==0)
    		{
    			if (bidx==-1)
    			{
    				problem("<imagemap> must not have attributes and must not be empty");
    				pos=endidx+1;
    				continue;
    			}
    			int endimagemap=content.indexOf("</imagemap>", bidx);
    			if (endimagemap==-1)
    			{
    				problem("</imagemap> not found.");
    				pos=bidx+1;
    				continue;
    			}
    			if (bidx+1<endimagemap)
    			{
    				imagemaps.put(Integer.toString(++lastimagemap), content.substring(bidx+1,endimagemap));
    				res.append(CLIX_IMAGEMAP).append(lastimagemap).append("_");
    				pos=endimagemap+"</imagemap>".length();
    				continue;
    			}
    		}
    		else if (tag.compareTo("math")==0)
    		{
    			if (bidx==-1)
    			{
    				problem("<math> must not have attributes and must not be empty");
    				pos=endidx+1;
    				continue;
    			}
    			int endmath=content.indexOf("</math>", bidx);
    			if (endmath==-1)
    			{
    				problem("</math> not found.");
    				pos=bidx+1;
    				continue;
    			}
    			if (bidx+1<endmath)
    			{
    				maths.put(Integer.toString(++lastmath), content.substring(bidx+1,endmath));
    				res.append(CLIX_MATH).append(lastmath).append("_");
    				pos=endmath+"</math>".length();
    				continue;
    			}
    		}
    		else if (tag.compareTo("gallery")==0)
    		{
    			int endgallery=content.indexOf("</gallery>", bidx);
    			if (endgallery==-1)
    			{
    				problem("</gallery> not found.");
    				pos=bidx+1;
    				continue;
    			}
    			if (bidx+1<endgallery)
    			{
    				String images=content.substring(bidx+1, endgallery);
    				String imgs[]=images.split("\n");
    				for (int im=0;im<imgs.length;im++)
    				{
    					if ((imgs[im].length()>0)&&(imgs[im].startsWith("Image:")))
    					{
    						res.append("[[").append(imgs[im]).append("]]\n");
    					}
    					else problem("<gallery> line with nonimage content ignored.");
    				}
    				pos=endgallery+"</gallery>".length();
    				continue;
    			}
    		}
    		else if (tag.compareTo("cite")==0)
    		{
    			if (endidx!=bidx)
    			{
    				// nonempty cite tag
    				// all we need here are the attributes
    				cites.put(Integer.toString(++lastcite), content.substring(endidx, bidx));
    				res.append(" "+CLIX_CITE).append(lastcite).append("_");
    			}
    			else
    			{
    				cites.put(Integer.toString(++lastcite), "");
    				res.append(" "+CLIX_CITE).append(lastcite).append("_");
    			}
    		}
    		else if (tag.compareTo("/cite")==0)
    		{
    			res.append(CLIX_ENDCITE);
    		}
    		else if (tag.compareTo("timeline")==0)
    		{
    			// absolutely no support for timelines in this version
    			
    			int finish=content.indexOf("</timeline>", bidx);
    			if (finish!=-1)
    			{
    				pos=finish+"</timeline>".length();
    				continue;
    			}
    			else
    			{
    				problem("broken timeline definition, ignored.");
    				pos=content.length();
    				continue;
    			}
    		}
    		else if (keepOriginalTags)
    		{
//        		System.out.println(tag);

        		if (fixOriginalTags==false)
        		{
        			res.append(content.subSequence(idx, bidx+1));
        		}
        		else
        		{
        			if (cellopen)
        				res.append("\n");
        			cellopen=false;
        			
        			if (tag.compareTo("table")==0)
        			{
        				res.append("\n{|");
        				if (type==2) res.append(content.subSequence(sidx, bidx));
        				res.append("\n");
        			}
        			else if (tag.compareTo("/table")==0)
        			{
        				res.append("|}\n");
        			}
        			else if (tag.compareTo("tr")==0)
        			{
        				res.append("\n|-");
        				if (type==2) res.append(content.subSequence(sidx, bidx));
        				res.append("\n");
        			}
        			else if (tag.compareTo("td")==0)
        			{
        				res.append("\n|");
        				if (type==2)
        				{
        					res.append(content.subSequence(sidx, bidx));
        					res.append("|");
        				}
        				cellopen=true;
        			}
        			else if (tag.compareTo("th")==0)
        			{
        				res.append("\n!");
        				if (type==2)
        				{
        					res.append(content.subSequence(sidx, bidx));
        					res.append("|");
        				}
        				cellopen=true;
        			}
        			else if (tag.compareTo("caption")==0)
        			{
        				res.append("\n|+");
        				if (type==2)
        				{
        					res.append(content.subSequence(sidx, bidx));
        					res.append("|");
        				}
        				cellopen=true;
        			}
        		}
    		}
    		
    		if (bidx==-1)
    		{
    			// no closing >, probably broken
    			pos=content.length();
    			break;
    		}
    		pos=bidx+1;
    	}
    	
    	pos=0;
    	
//    	content=res.toString();
//    	res.setLength(0);
//    	
//    	while (pos<content.length())
//    	{
//    		int idx=content.indexOf("\n ",pos);
//    		if (idx==-1)
//    		{
//    			res.append(content.subSequence(pos, content.length()));
//    			break;
//    		}
//    		
//    		res.append(content.subSequence(pos, idx));
//    		
//    		int endidx=content.indexOf('\n', idx+2);
//    		if (endidx==-1)
//    		{
//    			res.append(content.subSequence(idx+1, content.length()));
//    			break;
//    		}
//    		String prefix=content.substring(idx+2);
//    		if (content.charAt(idx+2)=='|')
//    		{
//    			// workaround for broken template and table definitions
//    			res.append(content.subSequence(idx+1, endidx));
//    			pos=endidx;
//    			continue;
//    		}
//    		else if (prefix.startsWith(" |"))
//    		{
//    			// workaround for broken template and table definitions
//    			res.append(content.subSequence(idx+1, endidx));
//    			pos=endidx;
//    			continue;
//    		}
//    		else if (prefix.startsWith("  |"))
//    		{
//    			// workaround for broken template and table definitions
//    			res.append(content.subSequence(idx+1, endidx));
//    			pos=endidx;
//    			continue;
//    		}
//			nowikis.put(Integer.toString(++lastnowiki), content.substring(idx+2,endidx+1));
//			res.append(CLIX_NOWIKI).append(lastnowiki).append("_\n");
//			pos=endidx;
//    	}
    	return res.toString();
    }
    
    private void fixReferences(String content, StringBuffer res)
    {
//    	problem("references.");
    	int pos=0;
    	while (pos<content.length())
    	{
    		int nref=content.indexOf(CLIX_REFERENCE,pos);
    		int nreflist=content.indexOf(CLIX_REFERENCE_LIST, pos);
    		
    		if ((nref==-1)&&(nreflist==-1))
    		{
    			res.append(content.subSequence(pos, content.length()));
    			break;
    		}
    		
    		if ((nref==-1)&&(nreflist!=-1))
    		{
        		res.append(content.subSequence(pos, nreflist));

        		// format is GROUP_
        		
        		int startidx=nreflist+CLIX_REFERENCE_LIST.length();
        		int gidx=content.indexOf('_', startidx);
        		
        		String group=null;
        		if (gidx!=startidx)
        			group=content.substring(startidx,gidx);

        		if (group!=null)
        			openTagLn(res,WikiConstants.refListTag,"group=\""+group+"\"");
        		else
        		{
        			openTagLn(res,WikiConstants.refListTag);
        		}	
    			Iterator<Map.Entry<Integer, String>> it=references.entrySet().iterator();
    			while (it.hasNext())
    			{
    				Map.Entry<Integer, String> entry=it.next();
    				if (group!=null)
    				{
    					String g=refgroups.get(entry.getKey());
    					if (g==null) continue;
    					if (g.compareTo(group)!=0) continue;
    				}
    				openTag(res,WikiConstants.refListEntryTag,"id=\""+entry.getKey()+"\"");
    				handleContent(entry.getValue(),res);
//                    fixOpenEmphasisTags(res);
    				closeTagLn(res,WikiConstants.refListEntryTag);
    			}

        		closeTagLn(res,WikiConstants.refListTag);

        		pos=gidx+1;
    		}
    		else if ((nref!=-1)&&(nreflist==-1))
    		{
        		res.append(content.subSequence(pos, nref));
        		// format is NAME_GROUP_
        		
        		int startidx=nref+CLIX_REFERENCE.length();
        		int nidx=content.indexOf('_', startidx);
        		int gidx=content.indexOf('_', nidx+1);
        		
        		int id=0;
        		if (nidx!=startidx)
        		{
        			id=Integer.parseInt(content.substring(startidx,nidx));
        		}
    			
        		openTag(res,WikiConstants.refTag,"xlink:href=\"#xpointer(//"+WikiConstants.refListTag+"/"+WikiConstants.refListEntryTag+"[@id=%22"+id+"%22])\" xlink:type=\"simple\"");
        		res.append(id);
        		closeTag(res,WikiConstants.refTag);
        		
        		pos=gidx+1;
    		}
    		else if (nref>nreflist)
    		{
        		res.append(content.subSequence(pos, nreflist));

        		// format is GROUP_
        		
        		int startidx=nreflist+CLIX_REFERENCE_LIST.length();
        		int gidx=content.indexOf('_', startidx);
        		
        		String group=null;
        		if (gidx!=startidx)
        			group=content.substring(startidx,gidx);

        		if (group!=null)
        			openTagLn(res,WikiConstants.refListTag,"group=\""+group+"\"");
        		else
        		{
        			openTagLn(res,WikiConstants.refListTag);
        		}	
    			Iterator<Map.Entry<Integer, String>> it=references.entrySet().iterator();
    			while (it.hasNext())
    			{
    				Map.Entry<Integer, String> entry=it.next();
    				if (group!=null)
    				{
    					String g=refgroups.get(entry.getKey());
    					if (g==null) continue;
    					if (g.compareTo(group)!=0) continue;
    				}
    				openTag(res,WikiConstants.refListEntryTag,"id=\""+entry.getKey()+"\"");
    				handleContent(entry.getValue(),res);
//                    fixOpenEmphasisTags(res);
    				closeTagLn(res,WikiConstants.refListEntryTag);
    			}

        		closeTagLn(res,WikiConstants.refListTag);

        		pos=gidx+1;
    		}
    		else
    		{
        		res.append(content.subSequence(pos, nref));
        		// format is NAME_GROUP_
        		
        		int startidx=nref+CLIX_REFERENCE.length();
        		int nidx=content.indexOf('_', startidx);
        		int gidx=content.indexOf('_', nidx+1);
        		
        		int id=0;
        		if (nidx!=startidx)
        		{
        			id=Integer.parseInt(content.substring(startidx,nidx));
        		}
    			
        		openTag(res,WikiConstants.refTag,"xlink:href=\"#xpointer(//"+WikiConstants.refListTag+"/"+WikiConstants.refListEntryTag+"[@id=%22"+id+"%22])\" xlink:type=\"simple\"");
        		res.append(id);
        		closeTag(res,WikiConstants.refTag);
        		
        		pos=gidx+1;
    		}
    	}
    }
    
    private void fixNowikis(String content, StringBuffer res)
    {
    	int pos=0;
    	
    	while (pos<content.length())
    	{
    		int idx=content.indexOf(CLIX_NOWIKI,pos);
    		if (idx==-1)
    		{
    			res.append(content.subSequence(pos, content.length()));
    			break;
    		}
    		 
    		res.append(content.subSequence(pos, idx));
    		
    		int startidx=idx+CLIX_NOWIKI.length();
    		int gidx=content.indexOf('_', startidx);

    		if (gidx==-1)
    		{
    			problem("invalid "+CLIX_NOWIKI+" entry, ignored.");
    			pos=startidx+1;
    			continue;
    		}
    		String id=content.substring(startidx,gidx);
    		
    		res.append(XML.XMLify2(nowikis.get(id)));
    		
    		pos=gidx+1;
    		
    		continue;
    	}
    }

    private void fixMaths(String content, StringBuffer res)
    {
    	int pos=0;
    	
    	while (pos<content.length())
    	{
    		int idx=content.indexOf(CLIX_MATH,pos);
    		if (idx==-1)
    		{
    			res.append(content.subSequence(pos, content.length()));
    			break;
    		}
    		
    		res.append(content.subSequence(pos, idx));
    		
    		int startidx=idx+CLIX_MATH.length();
    		int gidx=content.indexOf('_', startidx);

    		if (gidx==-1)
    		{
    			problem("invalid "+CLIX_MATH+" entry, ignored.");
    			pos=startidx+1;
    			continue;
    		}

    		String id=content.substring(startidx,gidx);
    		
    		// maybe we should test first if the included text is well-formed xml...
    		res.append("<math>").append(XML.XMLify2(maths.get(id))).append("</math>");
    		
    		pos=gidx+1;
    		
    		continue;
    	}
    }

    private void fixCites(String content, StringBuffer res)
    {
    	int pos=0;
    	
    	while (pos<content.length())
    	{
    		int idx=content.indexOf(CLIX_CITE,pos);
    		if (idx==-1)
    		{
    			res.append(content.subSequence(pos, content.length()));
    			break;
    		}
    		
    		res.append(content.subSequence(pos, idx));
    		
    		int startidx=idx+CLIX_CITE.length();
    		int gidx=content.indexOf('_', startidx);

    		if (gidx==-1)
    		{
    			problem("invalid "+CLIX_CITE+" entry, ignored.");
    			pos=startidx+1;
    			continue;
    		}

    		String id=content.substring(startidx,gidx);
    		
    		// maybe we should test first if the included text is well-formed xml...
    		openTag(res,WikiConstants.citeTag,cites.get(id));
    		
    		pos=gidx+1;
    		
    		continue;
    	}
    	
    	content=res.toString();
    	res.setLength(0);
    	
    	pos=0;
    	
    	while (pos<content.length())
    	{
    		int idx=content.indexOf(CLIX_ENDCITE,pos);
    		if (idx==-1)
    		{
    			res.append(content.subSequence(pos, content.length()));
    			break;
    		}
    		
    		res.append(content.subSequence(pos, idx));

    		closeTag(res,WikiConstants.citeTag);

    		pos=idx+CLIX_ENDCITE.length();
    		continue;
    	}

    }
    private void fixImageMaps(String content, StringBuffer res)
    {
    	int pos=0;
    	
    	while (pos<content.length())
    	{
    		int idx=content.indexOf(CLIX_IMAGEMAP,pos);
    		if (idx==-1)
    		{
    			res.append(content.subSequence(pos, content.length()));
    			break;
    		}
    		
    		res.append(content.subSequence(pos, idx));
    		
    		int startidx=idx+CLIX_IMAGEMAP.length();
    		int gidx=content.indexOf('_', startidx);

    		if (gidx==-1)
    		{
    			problem("invalid "+CLIX_IMAGEMAP+" entry, ignored.");
    			pos=startidx+1;
    			continue;
    		}

    		String id=content.substring(startidx,gidx);
    		
    		String text=imagemaps.get(id);

    		String lines[]=text.split("\n");
			handleImageLink(lines[0],0,lines[0].length(),res);
    		pos=gidx+1;
    		
    		continue;
    	}
    }

    private void fixTemplates(String content, StringBuffer result)
    {
    	int pos=0;
    	
    	while (pos<content.length())
    	{
    		int idx=content.indexOf(CLIX_TEMPLATE,pos);
    		if (idx==-1)
    		{
    			result.append(content.subSequence(pos, content.length()));
    			break;
    		}
    		
    		result.append(content.subSequence(pos, idx));
    		
    		int startidx=idx+CLIX_TEMPLATE.length();
    		int gidx=content.indexOf('_', startidx);

    		String id=content.substring(startidx,gidx);
    		
    		String templateText=templates.get(id);
    		
    		while (templateText.indexOf("&amp;")!=-1)
    			templateText=XML.unXMLify(templateText);
    		
    		templateText=removeOnlyComments(templateText);
    		
    		templateText=fixHTMLTags(templateText,true);

    		// get the template's parameters
    		
			HashMap<String,String> namedParameters=new HashMap<String,String>();
			
			String parameter="";
			String value="";
			int numParam=0;
			
			boolean isParam=true;
			int numSquareBrackets=0;
			
			for (int x=0;x<templateText.length();x++)
			{
				char c=templateText.charAt(x);
				
				if (c=='[')
				{
					numSquareBrackets++;
					if (isParam) parameter+=c;
					else 		 value+=c;
				}
				else if (c==']')
				{
					numSquareBrackets--;
					if (isParam) parameter+=c;
					else 		 value+=c;
				}
				
				else if (c=='|')
				{
					if (numSquareBrackets==2)
					{
						if (isParam==false) value+="|";
					}
					else
					{
						// parameter separator
						
						if (parameter!=null) parameter=parameter.trim();
						if (value!=null) value=value.trim();
						
						// if we read only a parameter name and no '=' sign, this is actually
						// a value for an unnamed parameter
						
						if ((parameter.length()>0)&&(value.length()==0)&&(isParam==true))
						{
							value=parameter;
							parameter=null;
						}
						else if (parameter.length()==0)
						{
							parameter=null; // can this happen at all?
						}
						
						numParam++;
						
//						System.out.println("param="+parameter+", value="+value);
						
						if (parameter!=null) namedParameters.put(parameter.trim(),value.trim());
						else if (value.length()>0) namedParameters.put("param"+String.valueOf(numParam),value.trim());

						parameter="";
						value="";
						isParam=true;
					}
				}
				else if (c=='=')
				{
					// parameter-value-separator
					isParam=false;
				}
				else
				{
					if (isParam) parameter+=c;
					else 		 value+=c;
				}
			}
			
			// handle the last parameter
			
			if (parameter!=null) parameter=parameter.trim();
			if (value!=null) value=value.trim();

			if ((parameter.length()>0)&&(value.length()==0)&&(isParam))
			{
				value=parameter;
				parameter=null;
			}
			else if (parameter.length()==0)
			{
				parameter=null; // can this happen at all?
			}
			
			numParam++;
		
			if (parameter!=null) namedParameters.put(parameter.trim(),value.trim());
			else if (value.length()>0) namedParameters.put("param"+String.valueOf(numParam),value.trim());

			if (namedParameters.isEmpty()==false) numTemplatesWithParameters++;

			openTagLn(result,"template");
			
			openTag(result,"name");
			result.append(XML.XMLify2(templateNames.get(id).trim()));
			closeTagLn(result,"name");
			
			//System.out.println("number of parameters="+namedParameters.size());
			
			if (namedParameters.size()>0)
			{
				openTagLn(result,"parameters");
			
				if (namedParameters.containsKey("image"))
				{
					String image=namedParameters.remove("image");
					String caption=namedParameters.remove("caption");
					
					String imagewidth=namedParameters.remove("image_width");
					
					handleContent("[[Image:"+image+"|"+(imagewidth==null?"":imagewidth+"|")+caption+"]]",result);					
				}
				Iterator<Map.Entry<String,String>> it=namedParameters.entrySet().iterator();
				while (it.hasNext())
				{
					Map.Entry<String,String> e=it.next();
					if (e.getValue().length()==0)
					{
						//emptyTagLn(result,XML.Tagify(e.getKey()));
					}
					else
					{
						openTag(result,XML.Tagify(e.getKey()));
						handleContent(e.getValue(),result);
						closeTagLn(result,XML.Tagify(e.getKey()));
					}
				}
				closeTagLn(result,"parameters");
			}
			closeTagLn(result,"template");

    		pos=gidx+1;
    		
    		continue;
    	}
    }

    private void embedTemplates(String content, StringBuffer result)
    {
    	Iterator<Map.Entry<String,String>> it=templates.entrySet().iterator();
    	
    	while (it.hasNext())
    	{
    		Map.Entry<String,String> e=it.next();
    		String templateText=e.getValue();
    		
    		while (templateText.indexOf("&amp;")!=-1)
    			templateText=XML.unXMLify(templateText);
    		
    		templateText=removeOnlyComments(templateText);
    		
    		templateText=fixHTMLTags(templateText,false);

    		// get the template's parameters
    		
			HashMap<String,String> namedParameters=new HashMap<String,String>();
			
			String parameter="";
			String value="";
			int numParam=0;
			
			boolean isParam=true;
			int numSquareBrackets=0;
			
			for (int x=0;x<templateText.length();x++)
			{
				char c=templateText.charAt(x);
				
				if (c=='[')
				{
					numSquareBrackets++;
					if (isParam) parameter+=c;
					else 		 value+=c;
				}
				else if (c==']')
				{
					numSquareBrackets--;
					if (isParam) parameter+=c;
					else 		 value+=c;
				}
				
				else if (c=='|')
				{
					if (numSquareBrackets>0)
					{
						if (isParam==false) value+="|";
					}
					else
					{
						// parameter separator
						
						if (parameter!=null) parameter=parameter.trim();
						if (value!=null) value=value.trim();
						
						// if we read only a parameter name and no '=' sign, this is actually
						// a value for an unnamed parameter
						
						if ((parameter.length()>0)&&(value.length()==0)&&(isParam==true))
						{
							value=parameter;
							parameter=null;
						}
						else if (parameter.length()==0)
						{
							parameter=null; // can this happen at all?
						}
						
						numParam++;
						
//						System.out.println("param="+parameter+", value="+value);
						
						if (parameter!=null) namedParameters.put(parameter.trim(),value.trim());
//						else if (value.length()>0) namedParameters.put("param"+String.valueOf(numParam),value.trim());

						parameter="";
						value="";
						isParam=true;
					}
				}
				else if (c=='=')
				{
					// parameter-value-separator
					isParam=false;
				}
				else
				{
					if (isParam) parameter+=c;
					else 		 value+=c;
				}
			}
			
			// handle the last parameter
			
			if (parameter!=null) parameter=parameter.trim();
			if (value!=null) value=value.trim();

			if ((parameter.length()>0)&&(value.length()==0)&&(isParam))
			{
				value=parameter;
				parameter=null;
			}
			else if (parameter.length()==0)
			{
				parameter=null; // can this happen at all?
			}
			
			numParam++;
		
			if (parameter!=null) namedParameters.put(parameter.trim(),value.trim());
			else if (value.length()>0) namedParameters.put("param"+String.valueOf(numParam),value.trim());

			if (namedParameters.isEmpty()==false) numTemplatesWithParameters++;

			openTagLn(result,"template");
			
			openTag(result,"name");
			result.append(XML.XMLify2(templateNames.get(e.getKey()).trim()));
			closeTagLn(result,"name");
			
			//System.out.println("number of parameters="+namedParameters.size());
			
			if (namedParameters.size()>0)
			{
				openTagLn(result,"parameters");
			
				if (namedParameters.containsKey("image"))
				{
					String image=namedParameters.remove("image");
					String caption=namedParameters.remove("caption");
					
					String imagewidth=namedParameters.remove("image_width");
				
//					System.out.println("image="+image);
//					System.out.println("caption="+caption);
//					System.out.println("imagewidth="+imagewidth);
					
					String mycaption=imagewidth;
					if (mycaption==null) mycaption=caption;
					else if (caption!=null) mycaption+="|"+caption;
					
					if (image.startsWith("[[Image:")==false) handleContent("[[Image:"+image+(mycaption!=null?"|"+mycaption:"")+"]]",result);
					else handleContent(image,result);					
				}
				Iterator<Map.Entry<String,String>> it1=namedParameters.entrySet().iterator();
				while (it1.hasNext())
				{
					Map.Entry<String,String> e1=it1.next();
					if (e1.getValue().length()==0)
					{
						//emptyTagLn(result,XML.Tagify(e.getKey()));
					}
					else
					{
						openTag(result,XML.Tagify(e1.getKey()));
						handleContent(e1.getValue(),result);
						closeTagLn(result,XML.Tagify(e1.getKey()));
					}
				}
				closeTagLn(result,"parameters");
			}
			closeTagLn(result,"template");
    	}
    	
    	result.append("\n").append(content);
    }

	private String handleInternalTemplates(String content)
		{
			templates=new HashMap<String,String>();
			templateNames=new HashMap<String,String>();
			lasttemplateid=0;
			
			StringBuffer result=new StringBuffer();
			
			int pos=0;
			int start=0;
			int end=0;
			
			boolean didSomething=false;
			
			while (true)
			{
				pos=0;
				didSomething=false;
				result=new StringBuffer(content.length());
				
				while ((pos!=-1)&&(pos<content.length()))
				{
	
					// fast forward to the first occurrence of a template call
					
					start=content.indexOf("{{",pos);
					
					if (start!=-1)
					{
						end=content.indexOf("}}",start+2);
						if (end==-1) // buggy template invocation
						{
							pos=-1;
							continue;
						}
						int s2=content.indexOf("{{",start+2);
						
						if ((s2!=-1)&&(s2<end))
						{
							// there is another template invocation earlier, so handle this first
							result.append(content.substring(pos,start+1));
							pos=start+1;
							continue;
						}
	
						didSomething=true;
						result.append(content.substring(pos,start));
					
						String templateText=content.substring(start+2,end);
				
						// check if the template call is surrounded by <nowiki> tags; if so, ignore it
						// (and silently remove the template call from the output}
						
						boolean ignoreme=false;
						
						int nowikistart=content.substring(0,start).lastIndexOf("<nowiki>");
						if (nowikistart!=-1)
						{
							int nowikiend=content.substring(0,start).lastIndexOf("</nowiki>");
							if ((nowikiend==-1)||(nowikiend<nowikistart))
							{
								nowikiend=content.substring(end+2).indexOf("</nowiki>");
								if (nowikiend!=-1)
								{
									// ignore this template
									ignoreme=true;
								}
							}
						}
				
						if (ignoreme)
						{
							if (debugTemplates) System.out.println("ignore template invocation: "+templateText);
						}
						else
						{
	//						if (debugTemplates) System.out.println("template invocation: "+templateText);
							
							// expand the template using its definition, replacing the parameters
							
							String r=handleTemplate(templateText);
							r=replacePipes(r);
							
							result.append(r);
							
	//						System.out.println("BEFORE: "+content.substring(pos,start+1));
	//						System.out.println("AFTER:  "+content.substring(end+2));
						}
	//					if (content.substring(end+2).startsWith("}}"))
	//						System.out.println("potential problem.");
						
						result.append(content.substring(end+2));
						content=result.toString();
						
	//					int cnt=0;
	//					for (int x=0;x<content.length();x++)
	//					{
	//						if (content.charAt(x)=='{') cnt++;
	//						else if (content.charAt(x)=='}') cnt--;
	//					}
	//					if (cnt!=0)
	//					{
	//						System.out.println("unbalanced number of braces!");
	//					}
						result=new StringBuffer(content.length());
						pos=0;
						continue;
					}
					else break;
				}
				if (pos!=-1) result.append(content.substring(pos));
				pos=0;
				
				if (didSomething==true)
				{
					if (result.length()==0)
						System.out.println("oops");
					content=result.toString();
	//				System.out.println("****\n"+content+"***\n");
					continue;
				}
				else
					return getPipesBack(content);
			}
		}

	private String fixSomeHTMLTags(String content, boolean reset)
	    {
	    	StringBuffer res=new StringBuffer(content.length());
	    	
	    	if (reset)
	    	{
		    	nowikis=new HashMap<String,String>();
		    	lastnowiki=0;
		    	maths=new HashMap<String,String>();
		    	lastmath=0;
	    	}
	    	
	    	int pos=0;
	    	
	    	while ((pos>-1)&&(pos<content.length()))
	    	{
	    		int idx=content.indexOf('<', pos);
	    		
	    		if (idx==-1)
	    		{
	    			res.append(content.subSequence(pos, content.length()));
	    			break;
	    		}
	    		
	    		res.append(content.subSequence(pos, idx));
	    		
	    		if ((idx+1<content.length())&&(content.charAt(idx+1)!='/')&&(Character.isLetter(content.charAt(idx+1))==false))
	    		{
//	    			res.append(content.subSequence(idx, idx+1));
	    			pos=idx+1;
	    			continue;
	    		}
	    		int endidx=Integer.MAX_VALUE;
	    		
	    		int bidx=content.indexOf('>', idx);
	    		if ((bidx!=-1)&&(bidx<endidx))
	    		{
	    			endidx=bidx;
	    		}
	    		int fidx=content.indexOf("/>", idx);
	    		if ((fidx!=-1)&&(fidx<endidx))
	    		{
	    			endidx=fidx;
	    		}
	    		int sidx=content.indexOf(' ',idx);
	    		if ((sidx!=-1)&&(sidx<endidx))
	    		{
	    			endidx=sidx;
	    		}
	    		if (endidx==Integer.MAX_VALUE)
	    		{
	    			// we silently assume that this was a bogus tag opener and ignore it
	    			problem("cannot find end of tag");
//	    			res.append(content.subSequence(idx, content.length()));
	    			pos=idx+1;
	    			continue;
	    		}
	
	    		String tag=content.substring(idx+1, endidx);
	    		
//	    		System.out.println(tag);
	    		
	    		if (tag.compareTo("nowiki")==0)
	    		{
	    			if (bidx==-1)
	    			{
	    				problem("<nowiki> must not have attributes and must not be empty");
	    				pos=endidx+1;
	    				continue;
	    			}
	    			int endnowiki=content.indexOf("</nowiki>", bidx);
	    			if (endnowiki==-1)
	    			{
	    				problem("</nowiki> not found.");
	    				pos=bidx+1;
	    				continue;
	    			}
	    			if (bidx+1<endnowiki)
	    			{
	    				nowikis.put(Integer.toString(++lastnowiki), content.substring(bidx+1,endnowiki));
	    				res.append(CLIX_NOWIKI).append(lastnowiki).append("_");
	    				pos=endnowiki+"</nowiki>".length();
	    				continue;
	    			}
	    		}
	    		else if (tag.compareTo("pre")==0)
	    		{
	    			// we currently handle <pre> tags similarly to <nowiki> tags
	    			// this is not completely correct, but good enough for our application
	    			if (bidx==-1)
	    			{
	    				problem("<pre> must not have attributes and must not be empty");
	    				pos=endidx+1;
	    				continue;
	    			}
	    			int endnowiki=content.indexOf("</pre>", bidx);
	    			if (endnowiki==-1)
	    			{
	    				problem("</pre> not found.");
	    				pos=bidx+1;
	    				continue;
	    			}
	    			if (bidx+1<endnowiki)
	    			{
	    				nowikis.put(Integer.toString(++lastnowiki), content.substring(bidx+1,endnowiki));
	    				res.append(CLIX_NOWIKI).append(lastnowiki).append("_");
	    				pos=endnowiki+"</pre>".length();
	    				continue;
	    			}
	    		}
	    		else if (tag.compareTo("math")==0)
	    		{
	    			if (bidx==-1)
	    			{
	    				problem("<math> must not have attributes and must not be empty");
	    				pos=endidx+1;
	    				continue;
	    			}
	    			int endmath=content.indexOf("</math>", bidx);
	    			if (endmath==-1)
	    			{
	    				problem("</math> not found.");
	    				pos=bidx+1;
	    				continue;
	    			}
	    			if (bidx+1<endmath)
	    			{
	    				maths.put(Integer.toString(++lastmath), content.substring(bidx+1,endmath));
	    				res.append(CLIX_MATH).append(lastmath).append("_");
	    				pos=endmath+"</math>".length();
	    				continue;
	    			}
	    		}
	    		else
	    			res.append(content.subSequence(idx, endidx));
	    		
	    		if (bidx==-1)
	    		{
	    			// no closing >, probably broken
	    			pos=content.length();
	    			break;
	    		}
	    		pos=endidx;
	    	}
	    	
	    	pos=0;
	    	
	//    	content=res.toString();
	//    	res.setLength(0);
	//    	
	//    	while (pos<content.length())
	//    	{
	//    		int idx=content.indexOf("\n ",pos);
	//    		if (idx==-1)
	//    		{
	//    			res.append(content.subSequence(pos, content.length()));
	//    			break;
	//    		}
	//    		
	//    		res.append(content.subSequence(pos, idx));
	//    		
	//    		int endidx=content.indexOf('\n', idx+2);
	//    		if (endidx==-1)
	//    		{
	//    			res.append(content.subSequence(idx+1, content.length()));
	//    			break;
	//    		}
	//    		String prefix=content.substring(idx+2);
	//    		if (content.charAt(idx+2)=='|')
	//    		{
	//    			// workaround for broken template and table definitions
	//    			res.append(content.subSequence(idx+1, endidx));
	//    			pos=endidx;
	//    			continue;
	//    		}
	//    		else if (prefix.startsWith(" |"))
	//    		{
	//    			// workaround for broken template and table definitions
	//    			res.append(content.subSequence(idx+1, endidx));
	//    			pos=endidx;
	//    			continue;
	//    		}
	//    	 	else if (prefix.startsWith("  |"))
	//    		{
	//    			// workaround for broken template and table definitions
	//    			res.append(content.subSequence(idx+1, endidx));
	//    			pos=endidx;
	//    			continue;
	//    		}
	//			nowikis.put(Integer.toString(++lastnowiki), content.substring(idx+2,endidx+1));
	//			res.append(CLIX_NOWIKI).append(lastnowiki).append("_\n");
	//			pos=endidx;
	//    	}
	    	return res.toString();
	    }

	/* multithread support methods */
	
	class workitem
	{
		String title;
		String id;
		String lastmod;
		String editor;
		int    currentFile;
		String editorid;
		String content;
		String revision;
		
		public workitem(String t, String i, String l, String e, String eid, String c, String r,int cf)
		{
			title=t; id=i; lastmod=l; editor=e; content=c; editorid=eid; revision=r; currentFile=cf;
		}
	}
	
	public void run()
	{
		int cnt=0;
		
		System.out.println(Thread.currentThread().toString()+" running");
		try
		{
			while (true)
			{
				workitem w=Wiki2XML.queue.poll(1, TimeUnit.DAYS);
				
				if (w==null)
				{
					System.out.println("["+Thread.currentThread().getId()+"] have not got any work for a long time, retiring.");
					break;
				}
				
				if (w.title==null)
				{
					System.out.println("["+Thread.currentThread().getId()+"] received DONE message.");
					break;					
				}
				cnt++;
				
				currentAuthor=w.editor;
				currentAuthorID=w.editorid;
				currentID=w.id;
				currentRevisionID=w.revision;
				currentTimestamp=w.lastmod;
				currentTitle=w.title;
				currentFile=w.currentFile;
								
				long start=System.currentTimeMillis();
				if (w.content.length()>0) 
					currentContent=makeXML(w.content);
				else 
					currentContent="";
				
				boolean ignored=(currentContent.length()==0);
				
				long parse=System.currentTimeMillis();
				dumpAndResetArticle();
				long end=System.currentTimeMillis();
				
				String type="XML";
				
				if (lastArticleWasBuggy) type="DEFECT";
				
				else if ((w.content.length()>=10)&&(w.content.substring(0,10).toLowerCase().startsWith("#redirect")))
					type="REDIR";
				else if (ignored) 
					type="IGN";
				
				System.out.println("["+Thread.currentThread().getId()+"-"+cnt+"]\t"+fillup(w.title,30)+"\t"+type+"\tparse:"+toSeconds(parse-start)+"\tstore:"+toSeconds(end-parse));
			}
		}
		catch(Exception e)
		{
			
			e.printStackTrace();
			System.err.println(e);
		}
	}
	
	private String toSeconds(long millisecs)
	{
		long sec=millisecs/1000;
		long msec=millisecs%1000;
		
		return String.format("%d.%tLs", sec,msec);
	}
	
	private String fillup(String in, int len)
	{
		if (in.length()>=len) return in.substring(0, len);
		
		else
		{
			for (int i=in.length();i<len;i++)
				in+=" ";
			return in;
		}
	}
	
	private void problem(String cause)
	{
		System.err.println("["+Thread.currentThread().getId()+"] "+currentTitle+": "+cause);
	}
	
	public static void dumpArticles(String fname)
	{
		try
		{
			File outputFile = new File(fname);
			FileOutputStream stream=new FileOutputStream(outputFile);
			
			Writer out = new BufferedWriter(new OutputStreamWriter(stream,"UTF-8"));
			
			Iterator<Map.Entry<String, String>> it=articles.entrySet().iterator();
			for (int i=0;i<articles.size();i++)
			{
				Map.Entry<String,String> e=it.next();
				out.write(e.getKey()+"\t"+e.getValue()+"\n");
			}
			out.close();
			stream.close();
		}
		catch(Exception e)
		{
			System.out.println("cannot dump to "+fname+": "+e);
		}
	}
	
	public static void dumpRedirections(String fname)
	{
		try
		{
			File outputFile = new File(fname);
			FileOutputStream stream=new FileOutputStream(outputFile);
			
			Writer out = new BufferedWriter(new OutputStreamWriter(stream,"UTF-8"));
			
			Iterator<Map.Entry<String, String>> it=redirections.entrySet().iterator();
			for (int i=0;i<redirections.size();i++)
			{
				Map.Entry<String,String> e=it.next();
				out.write(e.getKey()+"\t"+e.getValue()+"\n");
			}
			out.close();
			stream.close();
		}
		catch(Exception e)
		{
			System.out.println("cannot dump to "+fname+": "+e);
		}
	}

	public static void readArticles(String inputfile)
	{
		System.out.println("preloading article ids (this may take a while, expect approximately 7.6 million entries)...");

		if (articles==null)
			articles=Collections.synchronizedMap(new HashMap<String,String>());
		
        InputStream stream=null;
        BufferedReader reader=null;
        
        long start=System.currentTimeMillis();
        
        try
        {
        	stream=new CBZip2InputStream(new FileInputStream(inputfile));
        	reader=new BufferedReader(new InputStreamReader(stream));

        	int cnt=0;
        	
        	while (true)
        	{
        		String line=reader.readLine();
        		
        		if (line==null)
        		{
        			break;
        		}
        		
//        		System.out.println(line);

        		String tokens[]=line.split("\t");
        		
        		if (tokens.length!=2) continue;
        		
        		articles.put(tokens[0], tokens[1]);
        		if (++cnt%100000==0)
        		{
        			long end=System.currentTimeMillis();
        			
        			System.out.println("["+cnt+"@"+(end-start)/1000.0+"] "+tokens[0]+" "+articles.size()+" articles found");
        		}
        	}
        }
        catch(Exception e)
        {
        	e.printStackTrace();
        	System.err.println(e);
        	e.printStackTrace();
        }

	}

	public static void readRedirections(String inputfile)
	{
		System.out.println("preloading redirections (this may take a while, expect approximately 3.2 million entries)...");

		if (redirections==null)
			redirections=Collections.synchronizedMap(new HashMap<String,String>());
		
        InputStream stream=null;
        BufferedReader reader=null;
        
        long start=System.currentTimeMillis();
        
        try
        {
        	stream=new CBZip2InputStream(new FileInputStream(inputfile));
        	reader=new BufferedReader(new InputStreamReader(stream));

        	int cnt=0;
        	
        	while (true)
        	{
        		String line=reader.readLine();
        		
        		if (line==null)
        		{
        			break;
        		}
        		
//        		System.out.println(line);

        		String tokens[]=line.split("\t");
        		
        		if (tokens.length!=2) continue;
        		
        		redirections.put(tokens[0], tokens[1]);
        		if (++cnt%100000==0)
        		{
        			long end=System.currentTimeMillis();
        			
        			System.out.println("["+cnt+"@"+(end-start)/1000.0+"] "+tokens[0]+" "+redirections.size()+" redirections found");
        		}
        	}
        }
        catch(Exception e)
        {
        	e.printStackTrace();
        	System.err.println(e);
        	e.printStackTrace();
        }

	}

	// annotations for all pages
	
	// maps unique wordnet ids to the corresponding concept string
	public static HashMap<Integer,String> id2concept=new HashMap<Integer,String>();

//	public static HashMap<String,HashSet<annotation>> annotations=new HashMap<String,HashSet<annotation>>();
	public static HashMap<String,annotation[]> annotations=new HashMap<String,annotation[]>();
	
	final static int ARRAYSIZE=50; // depends on the collection and on the YAGO version
	
	public static void readAnnotations(String inputfile)
	{
		System.out.println("preloading concept annotations (this may take a while, expect approximately 11 million entries)...");
		
        InputStream stream=null;
        BufferedReader reader=null;
        
        String lastpage="";
//        HashSet<annotation> set=null;
        annotation arr[]=null;
        
        long start=System.currentTimeMillis();
        
        try
        {
        	stream=new CBZip2InputStream(new FileInputStream(inputfile));
        	reader=new BufferedReader(new InputStreamReader(stream));

        	int cnt=0;
        	int pos=0;
        	
        	while (true)
        	{
        		String line=reader.readLine();
        		
        		if (line==null)
        		{
        			if (lastpage.length()>0) annotations.put(lastpage,Arrays.copyOf(arr, pos));
        			break;
        		}
        		
//        		System.out.println(line);

        		String tokens[]=line.split("\t");
        		
        		if (tokens.length!=4) continue;
        		
        		if (lastpage.compareTo(tokens[0])!=0)
        		{
        			// new page found

        			if (lastpage.length()>0) annotations.put(lastpage,Arrays.copyOf(arr, pos));
        			lastpage=tokens[0];
        			arr=new annotation[ARRAYSIZE];
        			pos=0;
//        			set=new HashSet<annotation>();
//        			annotations.put(lastpage,set);
        		}
        		
        		Integer id=Integer.valueOf(tokens[2]);
        		
        		tokens[1]=tokens[1].replaceAll("'", "");

        		if (id2concept.containsKey(id)==false)
        			id2concept.put(id, tokens[1]);
        		
//        		set.add(new annotation(id,Double.valueOf(tokens[3])));
        		arr[pos++]=new annotation(id,Double.valueOf(tokens[3]));
        		
        		if (++cnt%100000==0)
        		{
        			long end=System.currentTimeMillis();
        			
        			System.out.println("["+cnt+"@"+(end-start)/1000.0+"] "+tokens[0]+" "+annotations.size()+" pages done, "+id2concept.size()+" distinct concepts found");
        		}
        	}
        }
        catch(Exception e)
        {
        	e.printStackTrace();
        	System.err.println(e);
        	e.printStackTrace();
        }

	}

	private static HashMap<String,annotation[]> catAnnotations=new HashMap<String,annotation[]>();

	public static void readCategories(String inputfile)
	{
		System.out.println("preloading category annotations (should be fast)...");
		
        InputStream stream=null;
        BufferedReader reader=null;
        
        String lastpage="";
//        HashSet<annotation> set=null;
        annotation arr[]=null;
        
        long start=System.currentTimeMillis();
        
        try
        {
        	stream=new CBZip2InputStream(new FileInputStream(inputfile));
        	reader=new BufferedReader(new InputStreamReader(stream));

        	int cnt=0;
        	int pos=0;
        	
        	while (true)
        	{
        		String line=reader.readLine();
        		
        		if (line==null)
        		{
        			if (lastpage.length()>0) catAnnotations.put(lastpage,Arrays.copyOf(arr, pos));
        			break;
        		}
        		
//        		System.out.println(line);

        		String tokens[]=line.split("\t");
        		
        		if (tokens.length!=4) continue;
        		
        		if (lastpage.compareTo(tokens[0])!=0)
        		{
        			// new page found

        			if (lastpage.length()>0) catAnnotations.put(lastpage,Arrays.copyOf(arr, pos));
        			lastpage=tokens[0];
        			arr=new annotation[ARRAYSIZE];
        			pos=0;
//        			set=new HashSet<annotation>();
//        			annotations.put(lastpage,set);
        		}
        		
        		tokens[1]=tokens[1].replaceAll("'", "");
        		Integer id=Integer.valueOf(tokens[2]);
        		
        		if (id2concept.containsKey(id)==false)
        			id2concept.put(id, tokens[1]);
        		
//        		set.add(new annotation(id,Double.valueOf(tokens[3])));
        		arr[pos++]=new annotation(id,Double.valueOf(tokens[3]));
        		
        		if (++cnt%100000==0)
        		{
        			long end=System.currentTimeMillis();
        			
        			System.out.println("["+cnt+"@"+(end-start)/1000.0+"] "+tokens[0]+" "+catAnnotations.size()+" pages done, "+id2concept.size()+" distinct concepts found");
        		}
        	}
        }
        catch(Exception e)
        {        	
        	System.err.println(e);
        	e.printStackTrace();
        }

	}

}
