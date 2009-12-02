package de.mpii.clix.wikipedia;

import java.io.*;

public class YAGOArticleDumper {

	/**
	 * @param args
	 */

	public static void main(String[] args)
	{
		String inputFile2="c:\\facts\\type\\ArticleExtractor.txt";
		String inputFile1="c:\\facts\\type\\IsAExtractor.txt";

		String fname="c:\\projects\\clix\\wikipedia\\input\\newannos.txt";
		
		File outputFile = new File(fname);

		File aFile=new File(inputFile1);
		File iFile=new File(inputFile2);
		
		int cnt=0;
		
	    try {

		      BufferedReader input1 =  new BufferedReader(new FileReader(aFile));
		      BufferedReader input2 =  new BufferedReader(new FileReader(iFile));
  		  
	      FileOutputStream stream=new FileOutputStream(outputFile);
			
		  Writer out = new BufferedWriter(new OutputStreamWriter(stream,"UTF-8"));
	      
		  try 
		  {
	        String line = null;

	        while (( line = input1.readLine()) != null){
	        	String tokens[]=line.split("\t");
//	        	System.out.println(tokens.length+" tokens");
	        	
	        	String title=tokens[1];
	        	String wordnet=tokens[2];
	        	if (wordnet.startsWith("wordnet_")==false) continue;
	        	wordnet=wordnet.substring(wordnet.indexOf('_')+1);
	        	String concept=wordnet.substring(0,wordnet.lastIndexOf('_'));
	        	wordnet=wordnet.substring(wordnet.lastIndexOf('_')+1);
	        	double confidence=Double.valueOf(tokens[3]);
	        	if (concept.startsWith("yago")==false)
	        	{
	        		try
	        		{
	        			out.write(title+"\t"+concept+"\t"+wordnet+"\t"+confidence+"\n");
	        			if (cnt++%1000==0)
	        				{System.out.println(cnt);
	        				}
	        		}
	        		catch(Exception e)
	        		{
			        	System.err.println(title+"\t"+concept+"\t"+wordnet+"\t"+confidence);	        		
	        			System.err.println("cannot insert: "+e);
	        		}
	        	}
	        }

	        while (( line = input2.readLine()) != null){
	        	String tokens[]=line.split("\t");
//	        	System.out.println(tokens.length+" tokens");
	        	
	        	String title=tokens[1];
	        	String wordnet=tokens[2];
	        	if (wordnet.startsWith("wordnet_")==false) continue;
	        	wordnet=wordnet.substring(wordnet.indexOf('_')+1);
	        	String concept=wordnet.substring(0,wordnet.lastIndexOf('_'));
	        	wordnet=wordnet.substring(wordnet.lastIndexOf('_')+1);
	        	double confidence=Double.valueOf(tokens[3]);
	        	if (concept.startsWith("yago")==false)
	        	{
	        		try
	        		{
	        			out.write(title+"\t"+concept+"\t"+wordnet+"\t"+confidence+"\n");
	        			if (cnt++%1000==0)
	        				{System.out.println(cnt);
	        				}
	        		}
	        		catch(Exception e)
	        		{
			        	System.err.println(title+"\t"+concept+"\t"+wordnet+"\t"+confidence);	        		
	        			System.err.println("cannot insert: "+e);
	        		}
	        	}
	        }

		  }
	      finally {
	    	  out.close();
	    	  stream.close();
		        input1.close();
		        input2.close();
	      }
	    }
	    catch (Exception ex){
	      ex.printStackTrace();
	    }
	    
	}

}
