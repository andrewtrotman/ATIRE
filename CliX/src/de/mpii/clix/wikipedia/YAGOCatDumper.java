package de.mpii.clix.wikipedia;

import java.io.*;

public class YAGOCatDumper {

	/**
	 * @param args
	 */

	public static void main(String[] args)
	{
		String inputFile1="c:\\facts\\subClassOf\\ConceptLinker.txt";

		String fname="c:\\projects\\clix\\wikipedia\\input\\catannos.txt";
		
		File outputFile = new File(fname);

		File aFile=new File(inputFile1);
		
		int cnt=0;
		
	    try {

		      BufferedReader input1 =  new BufferedReader(new FileReader(aFile));
  		  
	      FileOutputStream stream=new FileOutputStream(outputFile);
			
		  Writer out = new BufferedWriter(new OutputStreamWriter(stream,"UTF-8"));
	      
		  try 
		  {
	        String line = null;

	        while (( line = input1.readLine()) != null){
	        	String tokens[]=line.split("\t");
//	        	System.out.println(tokens.length+" tokens");
//	        	for (int i=0;i<tokens.length;i++) System.out.println(i+"\t"+tokens[i]);
	        	
	        	String cat=tokens[1];
	        	if (cat.startsWith("wikicategory_")==false) continue;
	        	cat=cat.substring("wikicategory_".length());
	        	String wordnet=tokens[2];
	        	if (wordnet.startsWith("wordnet_")==false) continue;
	        	wordnet=wordnet.substring("wordnet_".length());
	        	String concept=wordnet.substring(0,wordnet.lastIndexOf('_'));
	        	wordnet=wordnet.substring(wordnet.lastIndexOf('_')+1);
	        	double confidence=Double.valueOf(tokens[3]);
	        	if (concept.startsWith("yago")==false)
	        	{
	        		try
	        		{
	        			out.write(cat+"\t"+concept+"\t"+wordnet+"\t"+confidence+"\n");
	        			if (cnt++%1000==0)
	        				{System.out.println(cnt);
	        				}
	        		}
	        		catch(Exception e)
	        		{
			        	System.err.println(cat+"\t"+concept+"\t"+wordnet+"\t"+confidence);	        		
	        			System.err.println("cannot insert: "+e);
	        		}
	        	}
	        }

		  }
	      finally {
	    	  out.close();
	    	  stream.close();
		        input1.close();
	      }
	    }
	    catch (Exception ex){
	      ex.printStackTrace();
	    }
	    
	}

}
