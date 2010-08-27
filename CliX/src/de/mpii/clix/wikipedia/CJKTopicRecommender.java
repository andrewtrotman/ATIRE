package de.mpii.clix.wikipedia;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Map;
import java.util.Map.Entry;

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
	LinkedList<element> elementStack = new LinkedList<element>();;
	private String currentLang;
	private int articleCount;
	private int enLinkCount = 0;
	
	public static final String ENGLISH_LINK_MARKUP_S = "[[en:";
	
	private static Map<String, CJKTopic> articles = Collections.synchronizedMap(new HashMap<String,CJKTopic>());
	private static ArrayList<CJKTopic> cjk_topics = new ArrayList<CJKTopic>();
	private static ArrayList<ArticleCounter> counter = new ArrayList<ArticleCounter>();
	
	private class element
	{
		String tag;
		StringBuffer content;
	}
	
	private class ArticleCounter 
	{
		public ArticleCounter(String lang, int total, int i) {
			this.lang = lang;
			this.total = total;
			this.withEnLink = i;
		}
		
		String lang = null;
		int total = 0;
		int withEnLink = 0;
	}
	
	public class CJKTopicTitle {

		String id = null;
		String title = null;
		String lang = null;
		
		public CJKTopicTitle(String title, String id) {
			this.id = id;
			this.title = title;
			lang = "";
		}
		
		public CJKTopicTitle(String title, String id, String lang) {
			this.id = id;
			this.title = title;
			this.lang = lang;
		}
	}
	
	public class CJKTopic {
		ArrayList<CJKTopicTitle> titles = new ArrayList<CJKTopicTitle>();
		int size = 0;
		String enTitle = null;
	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		CJKTopicRecommender tr = new CJKTopicRecommender();
		tr.processFiles(args);
		tr.recommend();
		tr.list();
	}
	
	private void count() {
		counter.add(new ArticleCounter(currentLang, articleCount, enLinkCount));
	}
	
	private void status() {
		if (articleCount%10000==0)
			System.err.println("\n[0] read " + articleCount + ", " + articles.size() + " articles\n");
	}
	
	private void resetCounter() {
		enLinkCount = 0;
		articleCount = 0;
	}
	
	private void list() {
//		Iterator<Entry<String, CJKTopic>> it = articles.entrySet().iterator();
//		while (it.hasNext()) {
//			Entry<String, CJKTopic> pair = it.next();
//			
//			System.out.print(pair.getKey() + " ");
//			CJKTopic topic = (CJKTopic) pair.getValue();
//			for (int i = 0; i < topic.titles.size(); ++i) {
//				System.out.print(topic.titles.get(i) + " ");
//			}
//		}
		for (int i = 0; i < cjk_topics.size(); ++i) {
			CJKTopic topic = cjk_topics.get(i);
			System.out.print(topic.enTitle + ":");
			for (int j = 0; j < topic.titles.size(); ++j) {
				CJKTopicTitle title = topic.titles.get(j);
				System.out.print("<" + title.lang + ":" + title.id + "," + title.title + ">");
			}
			System.out.println("");
		}
	}

	private void recommend() {
		Iterator<Entry<String, CJKTopic>> it = articles.entrySet().iterator();
		while (it.hasNext()) {
			Entry<String, CJKTopic> pair = it.next();
			
			if (pair.getValue().titles.size() == 3)
				cjk_topics.add(pair.getValue());
		}
		//sort();
		Collections.sort(cjk_topics, new Comparator<CJKTopic>() {

		     public int compare(CJKTopic t1, CJKTopic t2) {
//		          CJKTopic t1 = (CJKTopic)o1;
//		          CJKTopic t2 = (CJKTopic)o2;
		   
		          if (t1.size == t2.size)
		             return t1.enTitle.compareTo(t2.enTitle);
		           return t1.size - t2.size;
		     }
		});
		
		
	}

	public void processFiles(String[] inputfile) {
		SAXParserFactory factory = SAXParserFactory.newInstance();
	    SAXParser parser;
		try {
			parser = factory.newSAXParser();

		    //WikiHandler handler = new WikiHandler();
		    boolean zippedInput = false;
			for(int i=0;i<inputfile.length;i++){				
			    if (inputfile[i].endsWith("bz2")) 
			    	zippedInput=true;
			    FileInputStream fis=new FileInputStream(inputfile[i]);			
		        InputStream stream=null;
		       // handler = new WikiHandler();
		        if (zippedInput) {
	    	        fis.skip(2);	
		        	stream=new CBZip2InputStream(fis);
		        }
		        else 
		        	stream=new BufferedInputStream(fis);

				parser.parse(stream, this);
				
				stream.close();
				count();
				status();
		        resetCounter();
		        parser = null;
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
				++articleCount;
				status();
			}
		}
	}
	
	private void addArticle(String content) {
		
    	int enLinkPos = content.indexOf(ENGLISH_LINK_MARKUP_S);
    	if (enLinkPos > 0) {
    		int titlePos = content.indexOf(":", enLinkPos);
	    	String namespace = content.substring(enLinkPos + 2, titlePos);
			
			int end = content.indexOf("]]", titlePos);
			if (end > 0) {
	        	String title = content.substring(titlePos + 1,end);
	        	
	        	if (title.indexOf(":") != -1){      	
	        		return;
	        		//title = title.substring(title.indexOf(":") + 1);
	        	}
	        	
	        	++enLinkCount;
				CJKTopic topic = new CJKTopic();
				topic.enTitle = title;
				topic.size = content.length();
				topic.titles.add(new CJKTopicTitle(currentTitle, currentID, currentLang));
				articles.put(title, topic);
			}
			else {
				System.err.println("Incomplete link for " + currentTitle);
			}
    	}
	}
}
