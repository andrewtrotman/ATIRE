package de.mpii.clix.wikipedia;

import java.io.*;
import java.util.*;
import java.sql.*;

import de.mpii.clix.support.Database;
import de.mpii.clix.support.UTF82ASCII;

public class YAGOReader {

	/**
	 * @param args
	 */

	public static void main(String[] args)
	{
//		String inputFile="c:\\facts\\type\\ArticleExtractor.txt";
		String inputFile="c:\\facts\\type\\IsAExtractor.txt";

		Database wikiDB;
		
        wikiDB=new Database();
        wikiDB.openDatabase("wikipedia", "wikipedia", "mpiat5316", "destiny.mpi");

        PreparedStatement insertCM=wikiDB.prepareStatement("insert into conceptmapinex2009(pagetitle,concept,conceptid,confidence) values(?,?,?,?)");
        
		File aFile=new File(inputFile);
		
		int cnt=0;
		
	    try {

	      BufferedReader input =  new BufferedReader(new FileReader(aFile));
	      try {
	        String line = null; //not declared within while loop

	        while (( line = input.readLine()) != null){
	        	String tokens[]=line.split("\t");
//	        	System.out.println(tokens.length+" tokens");
	        	
	        	String title=tokens[1];
	        	String wordnet=tokens[2];
	        	String header=wordnet.substring(0,wordnet.indexOf('_'));
	        	if (wordnet.startsWith("wordnet_")==false) continue;
	        	wordnet=wordnet.substring(wordnet.indexOf('_')+1);
	        	String concept=wordnet.substring(0,wordnet.lastIndexOf('_'));
	        	wordnet=wordnet.substring(wordnet.lastIndexOf('_')+1);
	        	double confidence=Double.valueOf(tokens[3]);
	        	if (concept.startsWith("yago")==false)
	        	{
	        		try
	        		{
	        			insertCM.setString(1,createFileName(title,"",false));
	        			insertCM.setString(2,concept);
	        			insertCM.setString(3,wordnet);
	        			insertCM.setDouble(4,confidence);
	        			insertCM.addBatch();
	        			if (cnt++%1000==0)
	        				{System.out.println(cnt);
	        				insertCM.executeBatch();
	        				}
	        		}
	        		catch(Exception e)
	        		{
			        	System.err.println(createFileName(title,"",false)+"\t"+concept+"\t"+wordnet+"\t"+confidence);	        		
	        			System.err.println("cannot insert: "+e);
	        		}
	        	}
	        }
	      }
	      finally {
	    	  insertCM.executeBatch();
	        input.close();
	      }
	    }
	    catch (Exception ex){
	      ex.printStackTrace();
	    }
	    
	}

	private static String createFileName(
			String filename,
			String owntitle,
			boolean buggy)
	{
		
		if (filename.length()==0) return "";
		
		String dir="";
		String lcname=filename.toLowerCase();
		filename = cleanInternalLink(filename);
		//System.out.println("new filename: "+filename);
		
		String new_filename="";

			// used only in links, and we handle image links separately
			
			String lctitle=owntitle.toLowerCase();
			
			String prefix="";
			
			if (lcname.startsWith("image:"))
			{
				if (lctitle.startsWith("image:")==false)
					prefix="../images/";
			}
			if (lcname.startsWith("template:"))
			{
				if (lctitle.startsWith("template:")==false)
					prefix="../templates/";
			}
			if (lcname.startsWith("category:"))
			{
				if (lctitle.startsWith("category:")==false)
					prefix="../categories/";
			}
			if (lcname.startsWith("wikipedia:"))
			{
				if (lctitle.startsWith("wikipedia:")==false)
					prefix="../wikipedia/";
			}
			else
			{
				// lcname does not start with any of our special namespaces
				if ((lctitle.startsWith("image:"))||(lctitle.startsWith("template:"))||(lctitle.startsWith("category:"))||(lctitle.startsWith("wikipedia:")))
					prefix="../pages/";	
			}
						
			// count the number of "/" characters in the page's title
			
			int num=0;
			char chars[]=owntitle.toCharArray();
			for (int i=0;i<chars.length;i++)
			{
				if (chars[i]=='/') num++;
			}

			// add the corresponding number of "../" steps to move to the base article's root directory
			for (int i=0;i<num;i++)
				new_filename+="../";

			// add the "../" to move to the namespace's root directory
			
			new_filename+="../";

			// if the current article is buggy, add yet another "../"
			
			if (buggy) new_filename+="../";

			// if we have to change dir, add the corresponding directory name

			new_filename=prefix+new_filename+getPrefix(filename)+"/"+filename+".xml";

			return new_filename;
	}

	private static String getPrefix(String wikiname) {
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

	// replace invalid characters with '_' (ignoring possible duplicates)
	// and add "$" to any uppercase character (but the first one)

	private static String cleanInternalLink(String text2clean) {

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

}
