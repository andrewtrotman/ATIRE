package de.mpii.clix.dblp;
import java.sql.*;
import java.io.*;
import java.util.*;

import de.mpii.clix.support.*;

public class DB2XML
{
    // file structure (note the trailing "/")
    
    private static String baseDir="c:\\projects\\clix\\dblp\\xml\\"; // make sure that this exists!
    private static String pubLinkPrefix="pubs/"; // dir is created if needed
    private static String peopleLinkPrefix="people/"; // dir is created if needed
    
    private static String ACMDir="y:/ACM/"; // base directory of the ACM collection
    
    // export control
    
    private static boolean exportPublications=true;
    private static boolean exportInproceedings=true;
    private static boolean exportArticles=true;
    private static boolean exportProceedings=true;
    private static boolean exportBooks=true;
    private static boolean exportIncollections=true;
    private static boolean exportPhdtheses=true;
    private static boolean exportMasterstheses=true;
    private static boolean exportWww=false;
    
    private static boolean exportPeople=false;
    private static boolean generateWWWLinks=true; // add wwwlink if available
    private static boolean generateHomepages=true;// add homepage if available
    
    private static boolean generateCitations=true;
    
    private static boolean generateAuthorLinks=true;
    private static boolean generateEditorLinks=true;
    private static boolean generateCrossrefLinks=false;

    private static boolean generateACMLinks=false;
    private static boolean generateUrlLinks=false;
    private static boolean generateEELinks=false;
    
    // database table names
    
    private String authorTable="Authors";
    private String editorTable="Editors";
    
    private String publicationTable="Publications";
    
    private String citationTable="Citations";
    
    private String linkTable="AuthorLinks"; // additional homepage links from BINGO!
    
    // populate wwwLinkTable with
    // insert into wwwlinks select author,url from publications p, authors a where p.key=a.key and p.type='www' and p.key like 'homepages/%' and url is not null
    
    private String wwwLinkTable="WWWLinks"; // links from www-entries that represent homepages

    // database access information
    
    private static String dbUser="dblp";
    private static String dbPasswd="dblp";
    private static String dbHost="mpino5314.ag5.mpi-sb.mpg.de";
    private static String dbService="pollux.mpi";

    private Database db;
    
    // prepared statements used by the publications exporter
    
    private PreparedStatement selectAuthors=null;
    private PreparedStatement selectEditors=null;
    private PreparedStatement selectCitations=null;
    
    // prepared statements used by the people exporter
    
    private PreparedStatement selectAuthoredTitles=null;
    private PreparedStatement selectEditedTitles=null;
    private PreparedStatement selectHomepage=null;
    private PreparedStatement selectWWWLink=null;

    // counters to show progress indicator
    
    private int numInproceedings=0;
    private int numArticle=0;
    private int numProceedings=0;
    private int numBook=0;
    private int numIncollection=0;
    private int numPhdthesis=0;
    private int numMastersthesis=0;
    private int numWww=0;
    private int numUnknown=0;
    private int numTotalPubs=0;
   
    private int numCitationLinks=0;
    private int numUrlLinks=0;
    private int numCrossrefLinks=0;
    private int numEeLinks=0;
    private int numAcmLinks=0;
    
    private int numTotalPeople=0;
    private int numAuthored=0;
    private int numEdited=0;
    private int numHomepageLinks=0;
    private int numWwwLinks=0;
    
    // progress indicator control
    
    private static boolean showProgressIndicator=true;
    private static int progressIndicatorFreq=100;

    // ----- main method -----
    
    public static void main(String[] args)
    {
        DB2XML c=new DB2XML();
        
        // checkParameters(c,args);
        
        c.db=new Database();
        c.db.openDatabase(dbUser,dbPasswd,dbHost,dbService);

        if (exportPublications) c.exportPublications();
        
        if (exportPeople) c.exportPeople();
    }
    
    // ----- export all selected publications -----
    
    private void exportPublications()
    {
        if ((exportInproceedings|exportProceedings|exportArticles|exportBooks|exportIncollections|exportPhdtheses|exportMasterstheses|exportWww)==false) return;
        
        selectAuthors=db.prepareStatement("select author from "+authorTable+" where key=?");
        selectEditors=db.prepareStatement("select editor from "+editorTable+" where key=?");
        selectCitations=db.prepareStatement("select targetkey,label from "+citationTable+" where sourcekey=?");
        
        String query="SELECT key,type,title,booktitle,crossref,pages,year,publisher,series,month,volume,number_,ee,cdrom,url,note,school,isbn from "+publicationTable+" WHERE type IN(";
        int cnt=0;
        
        if (exportInproceedings)
        {
            query+="'inproceedings'";
            cnt++;
        }
        if (exportProceedings)
        {
            if (cnt>0) query+=",";
            query+="'proceedings'";
            cnt++;
        }
        if (exportArticles)
        {
            if (cnt>0) query+=",";
            query+="'article'";
            cnt++;
        }
        if (exportBooks)
        {
            if (cnt>0) query+=",";
            query+="'book'";
            cnt++;
        }
        if (exportIncollections)
        {
            if (cnt>0) query+=",";
            query+="'incollection'";
            cnt++;
        }
        if (exportPhdtheses)
        {
            if (cnt>0) query+=",";
            query+="'phdthesis'";
            cnt++;
        }
        if (exportMasterstheses)
        {
            if (cnt>0) query+=",";
            query+="'mastersthesis'";
            cnt++;
        }
        if (exportWww)
        {
            if (cnt>0) query+=",";
            query+="'www'";
            cnt++;
        }
        query+=")";
        
        try
        {
            numTotalPubs=0;
            
            ResultSet rs=db.execQuery(query);
            
            while (rs.next())   
            {
                String key=rs.getString(1);
                String type=rs.getString(2);
                String title=rs.getString(3);
                String booktitle=rs.getString(4);
                String crossref=rs.getString(5);
                String pages=rs.getString(6);
                String year=rs.getString(7);
                String publisher=rs.getString(8);
                String series=rs.getString(9);
                String month=rs.getString(10);
                String volume=rs.getString(11);
                String number=rs.getString(12);
                String ee=rs.getString(13);
                String cdrom=rs.getString(14);
                String url=rs.getString(15);
                String note=rs.getString(16);
                String school=rs.getString(17);
                String isbn=rs.getString(18);
      
                exportPublication(key,type,title,booktitle,crossref,pages,year,publisher,series,
                        		  month,volume,number,ee,cdrom,url,note,school,isbn);
            
    	        if (showProgressIndicator)
    	        {
    	            updatePubStatus(type);
    	            if (numTotalPubs%progressIndicatorFreq==0)
    	                printPubStatus();
    	        }
            }
            rs.close();
        }
        catch(Exception e)
        {
            System.out.println("cannot read publications from database\nquery="+query+"\n"+e);
        }
    }
    
    // ----- export a single publication as XML file -----
    
    private void exportPublication(String key, String type, String title, String booktitle,
            					   String crossref, String pages, String year, String publisher,
            					   String series, String month, String volume, String number,
            					   String ee, String cdrom, String url, String note, String school,
            					   String isbn)
    {
        File f=generateFile(baseDir, generatePubLink(key));
		FileWriter xml=null;
		
		try
		{
		    xml=new FileWriter(f);

			xml.write("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
			xml.write("<!-- generated by CLiX/DBLP2XML $Revision: $ on "+Utils.getCurrentDate()+" -->\n");
		    xml.write("<"+type+" key=\""+key+"\"");
		    if (generateAuthorLinks|generateCrossrefLinks|generateEditorLinks|generateCitations|generateEELinks|generateACMLinks|generateUrlLinks)
		        xml.write(" xmlns:xlink=\""+XML.nsXLink+"\"");
		    xml.write(">\n");

		    exportPubAuthors(xml,key);
		    exportPubEditors(xml,key);

		    if (title!=null)
		    {
		        xml.write("<title>"+XML.XMLify(title)+"</title>\n");
		    }

		    if (booktitle!=null)
		    {
		        xml.write("<booktitle>"+XML.XMLify(booktitle)+"</booktitle>\n");
		    }

		    if (crossref!=null)
		    {
		        xml.write("<crossref");
		        if (generateCrossrefLinks)
		            xml.write(" xlink:type=\"simple\" xlink:href=\""+generateRelativePub2PubLink(key,crossref)+"\"");
		        xml.write(">"+XML.XMLify(crossref)+"</crossref>\n");
		        numCrossrefLinks++;
		    }

		    if (pages!=null)
		    {
		        xml.write("<pages>"+XML.XMLify(pages)+"</pages>\n");
		    }

		    if (year!=null)
		    {
		        xml.write("<year>"+XML.XMLify(year)+"</year>\n");
		    }

		    if (publisher!=null)
		    {
		        xml.write("<publisher>"+XML.XMLify(publisher)+"</publisher>\n");
		    }

		    if (series!=null)
		    {
		        xml.write("<series>"+XML.XMLify(series)+"</series>\n");
		    }

		    if (month!=null)
		    {
		        xml.write("<month>"+XML.XMLify(month)+"</month>\n");
		    }

		    if (volume!=null)
		    {
		        xml.write("<volume>"+XML.XMLify(volume)+"</volume>\n");
		    }

		    if (number!=null)
		    {
		        xml.write("<number>"+XML.XMLify(number)+"</number>\n");
		    }

		    if (ee!=null)
		    {
	            xml.write("<ee");
		        if ((generateEELinks)&&(ee.startsWith("http://")))
		        {
		            xml.write(" xlink:type=\"simple\" xlink:href=\""+ee+"\"");
		            numEeLinks++;
		        }
		        xml.write(">"+XML.XMLify(ee)+"</ee>\n");
		    }

		    if (cdrom!=null)
		    {
		        xml.write("<cdrom");
		        if (generateACMLinks)
		        {
		            xml.write(" xlink:type=\"simple\" xlink:href=\""+ACMDir+cdrom+"\"");
		            numAcmLinks++;
		        }
		        xml.write(">"+XML.XMLify(cdrom)+"</cdrom>\n");
		    }

		    if (url!=null)
		    {
		        xml.write("<url");
		        if ((generateUrlLinks)&&(url.startsWith("http://")))
		        {
		            xml.write(" xlink:type=\"simple\" xlink:href=\""+url+"\"");
		            numUrlLinks++;
		        }
		        xml.write(">"+XML.XMLify(url)+"</url>\n");
		    }

		    if (note!=null)
		    {
		        xml.write("<note>"+XML.XMLify(note)+"</note>\n");
		    }

		    if (school!=null)
		    {
		        xml.write("<school>"+XML.XMLify(school)+"</school>\n");
		    }

		    if (isbn!=null)
		    {
		        xml.write("<isbn>"+XML.XMLify(isbn)+"</isbn>\n");
		    }

		    if (generateCitations)
			    exportCitations(xml,key);
		    
		    xml.write("</"+type+">");
		    
		    xml.close();
		}
		catch(Exception e)
		{
		    System.out.println("cannot write to file "+f.getAbsolutePath());
		}

    }

    // generate all authors of a publication
    
    private void exportPubAuthors(FileWriter xml, String key)
    {
        try
        {
            selectAuthors.setString(1,key);
            
            ResultSet rs=selectAuthors.executeQuery();
            while (rs.next())
            {
                String author=rs.getString(1);
                xml.write("<author");
                if (generateAuthorLinks)
                    xml.write(" xlink:type=\"simple\" xlink:href=\""+generateRelativePub2PersonLink(key,author)+"\"");
                xml.write(">"+XML.XMLify(author)+"</author>\n");
            }
            rs.close();
        }
        catch(Exception e)
        {
            System.out.println("cannot load authors for "+key+"\n"+e);
        }
    }
   
    // generate all editors of a publication
    
    private void exportPubEditors(FileWriter xml, String key)
    {
        try
        {
            selectEditors.setString(1,key);
            
            ResultSet rs=selectEditors.executeQuery();
            while (rs.next())
            {
                String author=rs.getString(1);
                xml.write("<editor");
                if (generateEditorLinks)
                    xml.write(" xlink:type=\"simple\" xlink:href=\""+generateRelativePub2PersonLink(key,author)+"\"");
                xml.write(">"+XML.XMLify(author)+"</editor>\n");
            }
            rs.close();
        }
        catch(Exception e)
        {
            System.out.println("cannot load editors for "+key+"\n"+e);
        }
    }

    // generate all citations of a publication
    
    private void exportCitations(FileWriter xml, String key)
    {
        try
        {
            selectCitations.setString(1,key);
            
            ResultSet rs=selectCitations.executeQuery();
            while (rs.next())
            {
                String targetkey=rs.getString(1);
                String label=rs.getString(2);
                
                xml.write("<cite xlink:type=\"simple\" xlink:href=\""+generateRelativePub2PubLink(key,targetkey)+".xml\"");
                if (label!=null) xml.write(" label=\""+XML.XMLify(label)+"\"");
                xml.write("/>\n");
                
                numCitationLinks++;
            }
            rs.close();
        }
        catch(Exception e)
        {
            System.out.println("cannot load citations for "+key+"\n"+e);
        }
    }
    
    // ----- export all people -----
    
    private void exportPeople()
    {
        selectAuthoredTitles=db.prepareStatement("select a.key,title from "+publicationTable+" p, "+authorTable+" a where a.key=p.key and a.author=?");
        selectEditedTitles=db.prepareStatement("select e.key,title from "+publicationTable+" p, "+editorTable+" e where e.key=p.key and e.editor=?");
    
        selectHomepage=db.prepareStatement("select url from "+linkTable+" where author=?");
        selectWWWLink=db.prepareStatement("select url from "+wwwLinkTable+" where author=?");
        
        String query="SELECT distinct name from (select author as name from "+authorTable+") union all (select editor as name from "+editorTable+")";
 
        System.out.println();
        
        try
        {
            numTotalPeople=0;
            
            ResultSet rs=db.execQuery(query);
            
            while (rs.next())   
            {
                String name=rs.getString(1);
                
                exportPerson(name);
                
                numTotalPeople++;

                if (showProgressIndicator)
    	        {
    	            if (numTotalPeople%progressIndicatorFreq==0)
    	                printPeopleStatus();
    	        }
            }
            rs.close();
        }
        catch(Exception e)
        {
            System.out.println("cannot read people from database\nquery="+query+"\n"+e);
        }
    }
    
    // export a single person
    
    private void exportPerson(String name)
    {
        File f=generateFile(baseDir, generatePersonLink(name));
        
		FileWriter xml=null;
		
		try
		{
		    xml=new FileWriter(f);

			xml.write("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
			xml.write("<!-- generated by CLiX/DBLP2XML $Revision: $ on "+Utils.getCurrentDate()+" -->\n");
		    xml.write("<person xmlns:xlink=\""+XML.nsXLink+"\">\n");
		    xml.write("<name>"+XML.XMLify(name)+"</name>\n");
		    
		    if (generateHomepages) exportHomepage(xml,name);
		    if (generateWWWLinks) exportWWWLink(xml,name);
		    
		    exportPubs(xml,name);
		    
		    xml.write("</person>");
		    
		    xml.close();
		}
		catch(Exception e)
		{
		    System.out.println("cannot write to file "+f.getAbsolutePath());
		}
        
    }

    // generate all publications of a single person (authored + edited)
    
    private void exportPubs(FileWriter xml,String name)
    {
        try
        {
            int cnt=0;
            
            selectAuthoredTitles.setString(1,name);
            ResultSet rs=selectAuthoredTitles.executeQuery();
            while (rs.next())
            {
                if (cnt==0) xml.write("<authored>\n");
                String key=rs.getString(1);
                String title=rs.getString(2);
                xml.write("<publication xlink:type=\"simple\" xlink:href=\""+generateRelativePerson2PubLink(name,key)+".xml\">"+XML.XMLify2(title)+"</publication>\n");
                cnt++;
                numAuthored++;
            }
            rs.close();
            if (cnt>0) xml.write("</authored>\n");

            cnt=0;

            selectEditedTitles.setString(1,name);
            rs=selectEditedTitles.executeQuery();
            while (rs.next())
            {
                if (cnt==0) xml.write("<edited>\n");
                String key=rs.getString(1);
                String title=rs.getString(2);
                xml.write("<publication xlink:type=\"simple\" xlink:href=\""+generateRelativePerson2PubLink(name,key)+".xml\">"+XML.XMLify2(title)+"</publication>\n");
                cnt++;
                numEdited++;
            }
            rs.close();
            if (cnt>0) xml.write("</edited>\n");
        }
        catch(Exception e)
        {
            System.out.println("cannot export publications for "+name);
        }
    }
    
    // generate homepage link (from external table that has to be provided externally)
    
    private void exportHomepage(FileWriter xml,String name)
    {
        try
        {
            selectHomepage.setString(1,name);
            ResultSet rs=selectHomepage.executeQuery();
            while (rs.next())
            {
                String url=rs.getString(1);
                if (url==null) continue;
                if (url.startsWith("http://")==false) continue;
                
                xml.write("<homepage xlink:type=\"simple\" xlink:href=\""+XML.XMLify(url)+"\">"+XML.XMLify(url)+"</homepage>\n");
                
                numHomepageLinks++;
            }
            rs.close();
        }
        catch(Exception e)
        {
            System.out.println("cannot export homepage for "+name+"\n"+e);
        }
    }

    // generate link available from a www entry in DBLP
    
    private void exportWWWLink(FileWriter xml,String name)
    {
        try
        {
            selectWWWLink.setString(1,name);
            ResultSet rs=selectWWWLink.executeQuery();
            
            while (rs.next())
            {
                String url=rs.getString(1);
                if (url==null) continue;
                if (url.startsWith("http://")==false) continue;
                
                xml.write("<wwwlink xlink:type=\"simple\" xlink:href=\""+XML.XMLify(url)+"\">"+XML.XMLify(url)+"</wwwlink>\n");
                
                numWwwLinks++;
            }
            rs.close();
        }
        catch(Exception e)
        {
            System.out.println("cannot export WWW link for "+name+"\n"+e);
        }
    }
    
    // ----- support methods -----

    // generate link to a person's file relative to baseDir
    
    private String generatePersonLink(String author)
    {
        author.replaceAll("\\?","_");
        author.replaceAll("|","_");
        String name=author.substring(author.lastIndexOf(' ')+1);
        if (name.length()==0) name+="_";
        if (name.length()==1) name+="_";
        
        return peopleLinkPrefix+name.substring(0,1)+"/"+name.substring(1,2)+"/"+author+".xml";
    }

    // generate link to a publication's file relative to baseDir
    
    private String generatePubLink(String key)
    {
        return pubLinkPrefix+key+".xml";
    }
    
    // generate relative link from publication to person
    
    private String generateRelativePub2PersonLink(String pub, String person)
    {
        // we have to find out how many directories we have to climb up to the root dir
        
        pub=pubLinkPrefix+pub;
        
        int steps=-1;
        int index=0;
        while (index!=-1)
        {
            index=pub.indexOf('/',index+1);
            steps++;
        }
    
        String link="";
        while (steps>0)
        {
            link+="../";
            steps--;
        }
        return link+generatePersonLink(person);
    }
    
    // generate relative link from publication to publication
    // (this is not yet perfect)
    
    private String generateRelativePub2PubLink(String source, String target)
    {
        // we have to find out how many directories we have to climb up to the root dir
        
        int steps=-1;
        int index=0;
        while (index!=-1)
        {
            index=source.indexOf('/',index+1);
            steps++;
        }
    
        String link="";
        while (steps>0)
        {
            link+="../";
            steps--;
        }
        return link+target;
    }
    
    // generate relative link from person to publication
    
    private String generateRelativePerson2PubLink(String person, String pub)
    {
        // we have to find out how many directories we have to climb up to the root dir
        
        person=peopleLinkPrefix+"x/y/"+person;
        
        int steps=-1;
        int index=0;
        while (index!=-1)
        {
            index=person.indexOf('/',index+1);
            steps++;
        }
    
        String link="";
        while (steps>0)
        {
            link+="../";
            steps--;
        }
        return link+generatePubLink(pub);
    }
        
    // ----- generate a file and all its ancestor directories (if needed)
    
    private File generateFile(String basedir, String filename)
    {
        StringTokenizer tokenizer=new StringTokenizer(filename,"/",false);
        String name=basedir;
        int cnt=0;
        File f=null;
        
        while(tokenizer.hasMoreTokens())
        {
            String token=tokenizer.nextToken();
            name+=token;
            cnt++;
            
            try
            {
                if (name.endsWith(".xml")==false) name+="\\";
                f=new File(name);
                if (name.endsWith(".xml")==false)
                {
                    if (f.exists()==false) f.mkdir();
                }
                else
                    f.createNewFile();
            }
            catch(Exception e)
            {
                System.out.println("cannot create "+name+"\n"+e);
            }
        }
        
        return f;
    }
       
	// ----- progress indication -----
	
	private void updatePubStatus(String name)
	{
	    if (name.compareTo("inproceedings")==0)
	        numInproceedings++;
	    else if (name.compareTo("proceedings")==0)
	        numProceedings++;
	    else if (name.compareTo("article")==0)
	        numArticle++;
	    else if (name.compareTo("book")==0)
	        numBook++;
	    else if (name.compareTo("incollection")==0)
	        numIncollection++;
	    else if (name.compareTo("phdthesis")==0)
	        numPhdthesis++;
	    else if (name.compareTo("mastersthesis")==0)
	        numMastersthesis++;
	    else if (name.compareTo("www")==0)
	        numWww++;
	    else
	    {
	        numUnknown++;
	        System.out.println("unexpected tag: "+name);
	    }
	    
	    numTotalPubs++;
	}

	private void printPubStatus()
	{
	    System.out.println("TOT="+numTotalPubs+" INP="+numInproceedings+" PRO="+numProceedings+" ART="+numArticle+" BOO="+numBook+" INC="+numIncollection+" PHD="+numPhdthesis+" MAS="+numMastersthesis+" WWW="+numWww+" UNK="+numUnknown+" [CIT="+numCitationLinks+" CRF="+numCrossrefLinks+" URL="+numUrlLinks+" EEL="+numEeLinks+" ACM="+numAcmLinks+"]");
	}
	
	private void printPeopleStatus()
	{
	    System.out.println("TOT="+numTotalPeople+" AUT="+numAuthored+" EDI="+numEdited+" HOM="+numHomepageLinks+" WWW="+numWwwLinks);
	}
}
