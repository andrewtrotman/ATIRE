package de.mpii.clix.dblp;

import java.sql.*;
import org.apache.xerces.parsers.*;
import org.xml.sax.*;
import org.xml.sax.helpers.*;

import de.mpii.clix.support.*;

public class DBLP2DB extends DefaultHandler
{
    // database table names
    
    private String authorTable="Authors";
    private String editorTable="Editors";
    private String publicationTable="Publications";
    private String citationTable="Citations";
    
    // database access information
    
    private static String dbUser="dblp2008";
    private static String dbPasswd="dblp2008";
    private static String dbHost="localhost";
    private static String dbService="destiny.mpi";
    
    private static boolean generateFastIndexes=true;
    
    private Database db;
    
    private PreparedStatement exportAuthor;
    private PreparedStatement exportEditor;
    private PreparedStatement exportCitation;
    private PreparedStatement exportPublication;
    
    // control import of types (not yet implemented)
    
    private boolean importInproceedings=true;
    private boolean importArticles=true;
    private boolean importProceedings=true;
    private boolean importBooks=true;
    private boolean importIncollections=true;
    private boolean importPhdtheses=true;
    private boolean importMasterstheses=true;
    private boolean importWww=true;
    
    private boolean importCitations=true;
    
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
    
    // progress indicator control
    
    private static boolean showProgressIndicator=true;
    private static int progressIndicatorFreq=10000;
    
    // counters to support batched writes (and progress indication)
    
    private int numTotal=0;
    
    private int numAuthor=0;
    private int numEditor=0;
    private int numCitation=0;
    
    // input data
    
    private String inputFile="c:\\dblp.xml";
    
    // private class to capture publication info
    
    private class Publication
    {
        public String type;
        public String key;
        public String title="";
        public String booktitle="";
        public String crossref="";
        public String series="";
        public String volume="";
        public String number="";
        public String year="";
        public String month="";
        public String pages="";
        public String ee="";
        public String cdrom="";
        public String url="";
        public String school="";
        public String address="";
        public String note="";
        public String isbn="";
        public String publisher="";
    }
    
    private Publication currentPub;
    private String currentElement="";
    public String currentAuthor="";
    public String currentEditor="";
    public String currentCitationLabel="";
    public String currentCitationTarget="";
    
    private SAXParser saxParser=null;
    
    public static void main(String[] args)
    {
        DBLP2DB c=new DBLP2DB();
        
        // checkParameters(c,args);
        
        c.db=new Database();
        c.db.openDatabase(dbUser,dbPasswd,dbHost,dbService);
        
        c.dropTables();
        c.createTables();
        c.createPreparedStatements();
        
        c.parse(c.inputFile);
        
        c.finalBatch();
        
        if (showProgressIndicator)
        {
            System.out.println("\nfinished");
            c.printStatus();
        }
        
        if (generateFastIndexes)
            c.createFastIndexes();
        else
            c.createCompactIndexes();
    }
    
    private boolean parse(String file)
    {
		try
		{
			saxParser = new SAXParser();
			saxParser.setContentHandler(this);
			saxParser.setErrorHandler(this);
		}
		catch (Exception e)
		{
			System.out.println("cannot create SAXParser:\n"+ e);
			return false;
		}

		try
		{
		    saxParser.parse(file);
		}
		catch(Exception e)
		{
		    System.out.println("parsing failed:\n"+e);
		    return false;
		}
		
		return true;
    }
    
    // ------ implementation of the ContentHandler methods ------
    
    private int nestingLevel=0;
    
	public void startElement(String uri,String localName,String qName,Attributes attrs)
	{
	    nestingLevel++;
	    
	    if (nestingLevel==2) // a new publication starts
	    {
	        currentPub=new Publication();
	        currentPub.type=localName;
	        
	        // we're only interested in the "key" attribute of publications
	        
	        currentPub.key=attrs.getValue("key");
	        if (currentPub.key==null) currentPub.key="";
	        
	        currentElement="";
	        currentAuthor="";
	        currentEditor="";
	        currentCitationTarget="";
	        currentCitationLabel="";
	    }
	    else if (nestingLevel==3) // an entry for the current publication starts
	    {
	        currentElement=localName;
	        
	        // we're only interested in attributes of cite elements
	        
	        if (currentElement.compareTo("cite")==0)
	        {
	            currentCitationLabel=attrs.getValue("label");
	            if (currentCitationLabel==null) currentCitationLabel="";
	        }
	    }
	}

	public void endElement(String namespaceURI,String localName,String qName)
	{
	    nestingLevel--;
	    
	    if (nestingLevel==2)
	    {
	        if (currentElement.compareTo("author")==0)
	        {
	            exportAuthor();
	            currentAuthor="";
	            numAuthor++;
	        }
	        else if (currentElement.compareTo("editor")==0)
	        {
	            exportEditor();
	            currentEditor="";
	            numEditor++;
	        }

	        else if (currentElement.compareTo("cite")==0)
	        {
	            //System.out.println("test: "+currentCitationTarget);
	            
	            if ((currentCitationTarget.compareTo("...")!=0)&&(currentCitationTarget.compareTo("")!=0))
	            {
	                exportCitation();
		            numCitation++;
	            }
	            currentCitationLabel="";
	            currentCitationTarget="";
	        }

	        currentElement="";
	    }
	    else if (nestingLevel==1)
	    {
	        exportPublication();
	        
	        if (showProgressIndicator)
	        {
	            updateStatus(currentPub.type);
	            if (numTotal%progressIndicatorFreq==0)
	                printStatus();
	        }

	        currentPub=null;
	    }
	}
	
	public void characters(char[] ch, int start, int len)
	{
		String cont = "";

		for (int i = 0; i < len; i++) {
			cont = cont + ch[start + i];
		}
		
		if (currentElement.compareTo("cite")==0)
		{
		    currentCitationTarget+=cont;
		}
		else if (currentElement.compareTo("author")==0)
		{
		    currentAuthor+=cont;
		}
		else if (currentElement.compareTo("editor")==0)
		{
		    currentEditor+=cont;
		}
		else if (currentElement.compareTo("title")==0)
		{
		    currentPub.title+=cont;
		}
		else if (currentElement.compareTo("booktitle")==0)
		{
		    currentPub.booktitle+=cont;
		}
		else if (currentElement.compareTo("year")==0)
		{
		    currentPub.year+=cont;
		}
		else if (currentElement.compareTo("month")==0)
		{
		    currentPub.month+=cont;
		}
		else if (currentElement.compareTo("pages")==0)
		{
		    currentPub.pages+=cont;
		}
		else if (currentElement.compareTo("isbn")==0)
		{
		    currentPub.isbn+=cont;
		}
		else if (currentElement.compareTo("series")==0)
		{
		    currentPub.series+=cont;
		}
		else if (currentElement.compareTo("volume")==0)
		{
		    currentPub.volume+=cont;
		}
		else if (currentElement.compareTo("number")==0)
		{
		    currentPub.number+=cont;
		}
		else if (currentElement.compareTo("publisher")==0)
		{
		    currentPub.publisher+=cont;
		}
		else if (currentElement.compareTo("school")==0)
		{
		    currentPub.school+=cont;
		}
		else if (currentElement.compareTo("address")==0)
		{
		    currentPub.address+=cont;
		}
		else if (currentElement.compareTo("cdrom")==0)
		{
		    currentPub.cdrom+=cont;
		}
		else if (currentElement.compareTo("ee")==0)
		{
		    currentPub.ee+=cont;
		}
		else if (currentElement.compareTo("url")==0)
		{
		    currentPub.url+=cont;
		}
		else if (currentElement.compareTo("note")==0)
		{
		    currentPub.note+=cont;
		}
		else if (currentElement.compareTo("crossref")==0)
		{
		    currentPub.crossref+=cont;
		}
	}
	
	private void exportAuthor()
	{
	    try
	    {
	        exportAuthor.setString(1,currentPub.key);
	        exportAuthor.setString(2,currentAuthor);

	        exportAuthor.addBatch();

	        if (numAuthor%1000==0) exportAuthor.executeBatch();
	    }
	    catch(Exception e)
	    {
	        System.out.println("cannot export author "+currentAuthor+" for "+currentPub.key+":\n"+e);
	    }
	}
	
	private void exportEditor()
	{
	    try
	    {
	        exportEditor.setString(1,currentPub.key);
	        exportEditor.setString(2,currentEditor);

	        exportEditor.addBatch();
	        
	        if (numEditor%1000==0) exportEditor.executeBatch();
	    }
	    catch(Exception e)
	    {
	        System.out.println("cannot export editor "+currentEditor+" for "+currentPub.key+":\n"+e);
	    }
	}
	
	private void exportCitation()
	{
	    if ((currentCitationTarget.startsWith("conf/")==false)&&(currentCitationTarget.startsWith("journals/")==false)&&(currentCitationTarget.startsWith("tr/")==false)&&(currentCitationTarget.startsWith("books/")==false)&&(currentCitationTarget.startsWith("phd/")==false)&&(currentCitationTarget.startsWith("ms/")==false)&&(currentCitationTarget.startsWith("persons/")==false)&&(currentCitationTarget.startsWith("www/")==false))
	    {
	        System.out.println("unexpected or misformed citation '"+currentCitationTarget+"' in pub "+currentPub.key+"; label="+currentCitationLabel);
	    }
	    
	    try
	    {
	        exportCitation.setString(1,currentPub.key);
	        exportCitation.setString(2,currentCitationTarget);

	        if (currentCitationLabel.compareTo("")!=0)
	            exportCitation.setString(3,currentCitationLabel);
	        else
	            exportCitation.setNull(3,java.sql.Types.VARCHAR);
	        
	        exportCitation.addBatch();

	        if (numCitation%1000==0) exportCitation.executeBatch();
}
	    catch(Exception e)
	    {
	        System.out.println("cannot export citation of "+currentCitationTarget+" for "+currentPub.key+":\n"+e);
	    }
	}

	private void exportPublication()
	{
	    try
	    {
	        exportPublication.setString(1,currentPub.key);
	        exportPublication.setString(2,currentPub.type);

	        if (currentPub.title.compareTo("")!=0)
	            exportPublication.setString(3,currentPub.title);
	        else
	            exportPublication.setNull(3,java.sql.Types.VARCHAR);

	        if (currentPub.booktitle.compareTo("")!=0)
	            exportPublication.setString(4,currentPub.booktitle);
	        else
	            exportPublication.setNull(4,java.sql.Types.VARCHAR);

	        if (currentPub.crossref.compareTo("")!=0)
	            exportPublication.setString(5,currentPub.crossref);
	        else
	            exportPublication.setNull(5,java.sql.Types.VARCHAR);

	        if (currentPub.pages.compareTo("")!=0)
	            exportPublication.setString(6,currentPub.pages);
	        else
	            exportPublication.setNull(6,java.sql.Types.VARCHAR);

	        if (currentPub.year.compareTo("")!=0)
	            exportPublication.setString(7,currentPub.year);
	        else
	            exportPublication.setNull(7,java.sql.Types.VARCHAR);

	        if (currentPub.publisher.compareTo("")!=0)
	            exportPublication.setString(8,currentPub.publisher);
	        else
	            exportPublication.setNull(8,java.sql.Types.VARCHAR);

	        if (currentPub.series.compareTo("")!=0)
	            exportPublication.setString(9,currentPub.series);
	        else
	            exportPublication.setNull(9,java.sql.Types.VARCHAR);

	        if (currentPub.month.compareTo("")!=0)
	            exportPublication.setString(10,currentPub.month);
	        else
	            exportPublication.setNull(10,java.sql.Types.VARCHAR);

	        if (currentPub.volume.compareTo("")!=0)
	            exportPublication.setString(11,currentPub.volume);
	        else
	            exportPublication.setNull(11,java.sql.Types.VARCHAR);

	        if (currentPub.number.compareTo("")!=0)
	            exportPublication.setString(12,currentPub.number);
	        else
	            exportPublication.setNull(12,java.sql.Types.VARCHAR);

	        if (currentPub.ee.compareTo("")!=0)
	            exportPublication.setString(13,currentPub.ee);
	        else
	            exportPublication.setNull(13,java.sql.Types.VARCHAR);

	        if (currentPub.cdrom.compareTo("")!=0)
	            exportPublication.setString(14,currentPub.cdrom);
	        else
	            exportPublication.setNull(14,java.sql.Types.VARCHAR);

	        if (currentPub.url.compareTo("")!=0)
	            exportPublication.setString(15,currentPub.url);
	        else
	            exportPublication.setNull(15,java.sql.Types.VARCHAR);

	        if (currentPub.note.compareTo("")!=0)
	            exportPublication.setString(16,currentPub.note);
	        else
	            exportPublication.setNull(16,java.sql.Types.VARCHAR);

	        if (currentPub.school.compareTo("")!=0)
	            exportPublication.setString(17,currentPub.school);
	        else
	            exportPublication.setNull(17,java.sql.Types.VARCHAR);

	        if (currentPub.isbn.compareTo("")!=0)
	            exportPublication.setString(18,currentPub.isbn);
	        else
	            exportPublication.setNull(18,java.sql.Types.VARCHAR);

	        exportPublication.addBatch();
	        
	        if (numTotal%1000==0) exportPublication.executeBatch();
	    }
	    catch(Exception e)
	    {
	        System.out.println("cannot export publication "+currentPub.key+":\n"+e);
	    }
	}
	
	// ----- database support -----
	
    private void createTables()
    {
        db.execUpdate("CREATE TABLE "+authorTable+"(key VARCHAR2(200),author VARCHAR2(200))");
        db.execUpdate("CREATE TABLE "+editorTable+"(key VARCHAR2(200),editor VARCHAR2(200))");

        db.execUpdate("CREATE TABLE "+publicationTable+"(key VARCHAR2(200),type VARCHAR2(20),title VARCHAR2(600),booktitle VARCHAR2(600),crossref VARCHAR2(200),pages VARCHAR2(100),year VARCHAR2(200),publisher VARCHAR2(200),series VARCHAR2(200),month VARCHAR2(100),volume VARCHAR2(100),number_ VARCHAR2(100),ee VARCHAR2(200),cdrom VARCHAR2(200),url VARCHAR2(200),note VARCHAR2(200),school VARCHAR2(200),isbn VARCHAR2(100))");

        if (importCitations) db.execUpdate("CREATE TABLE "+citationTable+"(sourcekey VARCHAR2(200) NOT NULL,targetkey VARCHAR2(200) NOT NULL,label VARCHAR2(200))");
    }
    
    private void dropTables()
    {
        db.execUpdate("DROP TABLE "+authorTable);
        db.execUpdate("DROP TABLE "+editorTable);
        
        db.execUpdate("DROP TABLE "+publicationTable);
        
        if (importCitations) db.execUpdate("DROP TABLE "+citationTable);
        
    }
    
    private void createPreparedStatements()
    {
        exportAuthor=db.prepareStatement("INSERT INTO "+authorTable+"(key,author) VALUES(?,?)");
        exportEditor=db.prepareStatement("INSERT INTO "+editorTable+"(key,editor) VALUES(?,?)");

        exportCitation=db.prepareStatement("INSERT INTO "+citationTable+"(sourcekey,targetkey,label) VALUES(?,?,?)");
        
        exportPublication=db.prepareStatement("INSERT INTO "+publicationTable+"(key,type,title,booktitle,crossref,pages,year,publisher,series,month,volume,number_,ee,cdrom,url,note,school,isbn) VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
    }
    
	private void finalBatch()
	{
	    try
	    {
	        exportAuthor.executeBatch();
	        exportEditor.executeBatch();
	        exportCitation.executeBatch();
	        exportPublication.executeBatch();
	    }
	    catch(Exception e)
	    {
	        System.out.println("final batched writes failed:\n"+e);
	    }
	}
	
	// this generates indexes that are optimized for query performance, not for size
	
	private void createFastIndexes()
	{
	    System.out.println("create author index on author table");
	    db.execUpdate("CREATE INDEX IDX_"+authorTable+"_AUTHOR ON "+authorTable+"(Author,Key)");

	    System.out.println("create key index on author table");
	    db.execUpdate("CREATE INDEX IDX_"+authorTable+"_KEY ON "+authorTable+"(Key,Author)");

	    System.out.println("create sourcekey index on citation table");
	    db.execUpdate("CREATE INDEX IDX_"+citationTable+"_SOURCEKEY ON "+citationTable+"(SourceKey,TargetKey,Label)");

	    System.out.println("create editor index on editor table");
	    db.execUpdate("CREATE INDEX IDX_"+editorTable+"_AUTHOR ON "+editorTable+"(Editor,Key)");

	    System.out.println("create key index on Editor table");
	    db.execUpdate("CREATE INDEX IDX_"+editorTable+"_KEY ON "+editorTable+"(Key,Editor)");

	    System.out.println("create key index on Publication table");
	    db.execUpdate("CREATE INDEX IDX_"+publicationTable+"_KEY ON "+publicationTable+"(Key,Title)");

	    System.out.println("create type index on Publication table");
	    db.execUpdate("CREATE INDEX IDX_"+publicationTable+"_TYPE ON "+publicationTable+"(Type)");
	}

	// this generates indexes that are optimized for size, not for query performance

	private void createCompactIndexes()
	{
	    System.out.println("create author index on author table");
	    db.execUpdate("CREATE INDEX IDX_"+authorTable+"_AUTHOR ON "+authorTable+"(Author)");

	    System.out.println("create key index on author table");
	    db.execUpdate("CREATE INDEX IDX_"+authorTable+"_KEY ON "+authorTable+"(Key)");

	    System.out.println("create sourcekey index on citation table");
	    db.execUpdate("CREATE INDEX IDX_"+citationTable+"_SOURCEKEY ON "+citationTable+"(SourceKey)");

	    System.out.println("create editor index on editor table");
	    db.execUpdate("CREATE INDEX IDX_"+editorTable+"_AUTHOR ON "+editorTable+"(Editor)");

	    System.out.println("create key index on Editor table");
	    db.execUpdate("CREATE INDEX IDX_"+editorTable+"_KEY ON "+editorTable+"(Key)");

	    System.out.println("create key index on Publication table");
	    db.execUpdate("CREATE INDEX IDX_"+publicationTable+"_KEY ON "+publicationTable+"(Key)");

	    System.out.println("create type index on Publication table");
	    db.execUpdate("CREATE INDEX IDX_"+publicationTable+"_TYPE ON "+publicationTable+"(Type)");
	}

	// ----- progress indication -----
	
	private void updateStatus(String name)
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
	    
	    numTotal++;
	}
	
	private void printStatus()
	{
	    System.out.println("TOT="+numTotal+" INP="+numInproceedings+" PRO="+numProceedings+" ART="+numArticle+" BOO="+numBook+" INC="+numIncollection+" PHD="+numPhdthesis+" MAS="+numMastersthesis+" WWW="+numWww+" UNK="+numUnknown+" [AUT="+numAuthor+" EDI="+numEditor+" CIT="+numCitation+")");
	}
}
