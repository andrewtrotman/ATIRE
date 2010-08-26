package de.mpii.clix.wikipedia;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.apache.tools.bzip2.CBZip2InputStream;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

import de.mpii.clix.support.XML;

public class CJKTopicRecommender extends DefaultHandler 
{
	String currentTitle=null;
	String currentContent=null;
	String currentID=null;
	LinkedList<element> elementStack;
	private String currentLang;
	
	private static Map<String, CJKTopic> articles = Collections.synchronizedMap(new HashMap<String,CJKTopic>()); 
	
	private class element
	{
		String tag;
		StringBuffer content;
	}
	
	public class CJKTopic {
		ArrayList<String> titles = new ArrayList<String>();
		int size = 0;
	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		CJKTopicRecommender tr = new CJKTopicRecommender();
		tr.processFiles(args);
		tr.recommend();
	}
	
	private void recommend() {
		
		
	}

	public void processFiles(String[] inputfile) {
		SAXParserFactory factory = SAXParserFactory.newInstance();
	    SAXParser parser;
		try {
			parser = factory.newSAXParser();

		    WikiHandler handler = new WikiHandler();
		    boolean zippedInput = false;
			for(int i=0;i<inputfile.length;i++){				
			    if (inputfile[i].endsWith("bz2")) 
			    	zippedInput=true;
			    FileInputStream fis=new FileInputStream(inputfile[i]);			
		        InputStream stream=null;
		        handler = new WikiHandler();
		        if (zippedInput) {
	    	        fis.skip(2);	
		        	stream=new CBZip2InputStream(fis);
		        }
		        else 
		        	stream=new BufferedInputStream(fis);
		        handler.setCurrentFile(i);
				handler.reset(1);
				parser.parse(stream,handler);
				
				stream.close();
			}
		} catch (ParserConfigurationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (SAXException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public void dumpTopics() {
		
	}
	
	private void reset() {
		currentTitle=null;
		currentContent=null;
		currentID=null;
	}
	
	public void characters(char[] text, int start, int length) throws SAXException
	{		
		element el=elementStack.getFirst();

		if (el.content!=null) 
			el.content.append(text,start,length);
		else 
			el.content=new StringBuffer(new String(text).substring(start,start+length));

	}
	
	public void startElement(String namespaceURI, String localName, String qualifiedName, Attributes arg3) throws SAXException
	{
		//System.out.println("startElement("+namespaceURI+","+localName+","+qualifiedName+")");
		
		// if there is already content available from the current element, output it to the target file
		
		// create a new element and put it on the stack
		
		element el=new element();
		el.tag=qualifiedName;
		
		elementStack.addFirst(el);
		
		if (elementStack.size() == 1) {
			currentLang = arg3.getValue("xml:lang");
			
//			if (articles.get(currentLang) == null)
//				articles.put(currentLang, Collections.synchronizedMap(new HashMap<String,String>()));
//			if (redirections.get(currentLang) == null)
//				redirections.put(currentLang, Collections.synchronizedMap(new HashMap<String,String>()));			
//			if (!languageLinks.contains(currentLang))
//				languageLinks.add(currentLang);
		}
	}
	
	public void endElement(String namespaceURI, String localName, String qualifiedName) throws SAXException
	{
		//System.out.println("endElement("+namespaceURI+","+localName+","+qualifiedName+")");

		element el=elementStack.removeFirst();
		
		if (el.content!=null)
		{
			
			if (el.tag.equals("text"))
			{
				addArticle(el.content.toString());
			}
			else if (el.tag.equals("title"))
			{
				currentTitle=el.content.toString();
			}
			else if (el.tag.equals("id"))
			{
				if (elementStack.size()==2)
					currentID=XML.XMLify2(el.content.toString());
			}
			else if (el.tag.equals("page"))
			{
				reset();
			}
		}
	}
	
	private void addArticle(String content) {
		
	}
}
