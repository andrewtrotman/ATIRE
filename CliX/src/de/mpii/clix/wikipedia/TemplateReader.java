package de.mpii.clix.wikipedia;

import org.xml.sax.*;

import java.util.*;
import java.io.*;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

import de.mpii.clix.support.*;

/*
 * reads all templates from an XML file and stores them for later use
 */

public class TemplateReader extends DefaultHandler
{
//	private String outputDir="/local/var/tmp/Wikipedia/TemplateStore";

	private String outputDir="c:\\YAWN2\\TemplateStore";

	private class element
	{
		String tag;
		StringBuffer content;
	}
	
	LinkedList<element> elementStack;

	SAXParser parser;
	DefaultHandler handler;
	
	int articleCount;
	int generatedCount;
	int abortedCount;
	int invalidCount;
	
	// global properties
	
	final boolean createFiles=true;
	
	public TemplateReader()
	{
		elementStack=new LinkedList<element>();
		
		try
		{
			SAXParserFactory factory = SAXParserFactory.newInstance();
		    parser = factory.newSAXParser();
		    handler = new DefaultHandler();
		}
		catch(Exception e)
		{
			System.out.println(e);
		}

	}
	
	int cnt=0;
	
	public void characters(char[] text, int start, int length) throws SAXException
	{
		//System.out.println("\ttext: \""+new String(text).substring(start,start+length)+"\" "+length+" bytes");
		
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
		if (articleCount%100!=0) status();
	}

	public void endElement(String namespaceURI, String localName, String qualifiedName) throws SAXException
	{
		//System.out.println("endElement("+namespaceURI+","+localName+","+qualifiedName+")");

		element el=elementStack.removeFirst();
		
		if (el.content!=null)
		{
			if (el.tag.equals("text"))
			{
				currentContent=el.content.toString();
			}
			else if (el.tag.equals("title"))
			{
				currentTitle=XML.XMLify2(el.content.toString());
				//if (phase==1) System.out.println(currentTitle);
				//if ((phase==1)&&(currentTitle.equals("Forever Changes"))) debugTokens=true;
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
				if (currentTitle.startsWith("Template:"))
					dumpAndResetTemplate();

				articleCount++;
				if (articleCount%100==0) status();
			}
		}
	}

	private void dumpAndResetTemplate()
	{
		if ((currentContent!=null)&&(createFiles))
		{
			dumpXMLFile(currentTitle,currentContent);
		}
		
		currentTitle=null;
		currentAuthor=null;
		currentAuthorID=null;
		currentContent=null;
		currentID=null;
		currentRevisionID=null;
		currentTimestamp=null;
	}

	public void endPrefixMapping(String arg0) throws SAXException {
		// TODO Auto-generated method stub
		
	}

	public void ignorableWhitespace(char[] arg0, int arg1, int arg2) throws SAXException {
		// TODO Auto-generated method stub
		
	}

	public void processingInstruction(String arg0, String arg1) throws SAXException {
		// TODO Auto-generated method stub
		
	}

	public void setDocumentLocator(Locator arg0) {
		// TODO Auto-generated method stub
		
	}

	public void skippedEntity(String arg0) throws SAXException {
		// TODO Auto-generated method stub
		
	}

	public void startDocument() throws SAXException {
		// TODO Auto-generated method stub
		
	}

	String currentXML=null;
	String currentTitle=null;
	String currentContent=null;
	String currentID=null;
	String currentTimestamp=null;
	String currentRevisionID=null;
	String currentAuthorID=null;
	String currentAuthor=null;
	
	boolean isRedirect=false;
	
	public void startElement(String namespaceURI, String localName, String qualifiedName, Attributes arg3) throws SAXException
	{
		//System.out.println("startElement("+namespaceURI+","+localName+","+qualifiedName+")");
		
		// if there is already content available from the current element, output it to the target file
		
		// create a new element and put it on the stack
		
		element el=new element();
		el.tag=qualifiedName;
		
		elementStack.addFirst(el);
	}

	public void startPrefixMapping(String arg0, String arg1) throws SAXException {
		// TODO Auto-generated method stub
		
	}

	// replace invalid characters with '_' (ignoring possible duplicates)
	// and add "$" to any uppercase character (but the first one)

	private String cleanInternalLink(String text2clean) {

		if (text2clean.startsWith(".."))
			text2clean="dot2_"+text2clean;
		else if (text2clean.startsWith("."))
			text2clean="dot_"+text2clean;

		char[] ca = text2clean.replaceAll("\\s", "_").toCharArray();
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
	
	private String createFileName(
			String filename,
			boolean relative,
			String owntitle,
			boolean buggy)
	{
		
		if (filename.length()==0) return "";

		String lcname=filename.toLowerCase();
		if (!relative)
		{
			if (lcname.startsWith("template:")
					|| lcname.startsWith("模板:"))
			{
				filename=filename.substring(9);
			}
		}
		
		filename = cleanInternalLink(filename);
		//System.out.println("new filename: "+filename);
		
		String new_filename="";

		new_filename = outputDir + "/" + getPrefix(filename) + "/" + filename+".wkt";
		
		if (new_filename.length()>255)
		{
			System.out.println("filename '"+new_filename+"' is too long, truncate it");
			new_filename = new String(outputDir + "/" + getPrefix(filename) + "/" + filename).substring(0,251)+".wkt";
		}
	
		checkAllDirs(new_filename);

		return new_filename;
	}

	private boolean dumpXMLFile(
			String filename,
			String content)
	{
		System.out.println("dumpXMLFile("+filename+","+content.length()+" bytes)");
		
		generatedCount++;
		
		try {

			filename = createFileName(filename, false,null,false);

			System.out.println("filename="+filename);
			
			File outputFile = new File(filename);
			FileOutputStream stream=new FileOutputStream(outputFile);
			
			Writer out = new BufferedWriter(new OutputStreamWriter(stream,"UTF-8"));

			//new ByteArrayInputStream(content.getBytes());

			out.write(content);
			out.flush();
			out.close();
		}
		catch (Exception e)
		{
			System.out.println(filename);
			e.printStackTrace();
			generatedCount--;
			abortedCount++;
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

	private void status()
	{
		statusPhase1();
	}
	
	private void statusPhase1()
	{
		System.out.println("\n[1] read "+articleCount+", generated "+generatedCount+", aborted "+abortedCount+", invalid "+invalidCount+"\n");
	}
}
