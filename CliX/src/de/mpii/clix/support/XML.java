package de.mpii.clix.support;

public class XML
{
    public static final String nsXLink="http://www.w3.org/1999/xlink";
    
    public static String unXMLify(String input)
    {
    	if (input==null) return null;
		input=input.replaceAll("&amp;","&");
		input=input.replaceAll("&lt;","<");
		input=input.replaceAll("&gt;",">");
		input=input.replaceAll("&quot;","\"");
		input=input.replaceAll("&apos;","'");
    	
		return input;
    }
    
    public static String XMLify2(String input)
    {
    	if (input==null) return null;
		//input=input.replaceAll("&","&amp;");
		input=input.replaceAll("<","&lt;");
		input=input.replaceAll(">","&gt;");
		input=input.replaceAll("\"","&quot;");
		input=input.replaceAll("'","&apos;");

		return input;
    }

    public static String XMLify(String input)
    {
    	if (input==null) return null;
		input=input.replaceAll("&","&amp;");
		input=input.replaceAll("<","&lt;");
		input=input.replaceAll(">","&gt;");
		input=input.replaceAll("\"","&quot;");
		input=input.replaceAll("'","&apos;");

		return input;
    }

    public static String Tagify(String input)
    {
    	if (input==null) return null;
    	
    	input=input.trim();
    	
//    	if (Character.isDigit(input.charAt(0)))
//    		input="_"+input;
//    	
//    	input=input.replaceAll(" ","_");
//    	input=input.replaceAll("/","_");
//    	input=input.replaceAll("(","_");
//    	input=input.replaceAll(")","_");
//    	input=input.replaceAll("[","_");
//    	input=input.replaceAll("]","_");

    	char arr[]=input.toCharArray();
    	StringBuffer tag=new StringBuffer();
    	for (int i=0;i<arr.length;i++)
    	{
    		if (Character.isLetter(arr[i])) tag.append(arr[i]);
    		else if ((i>0)&&(Character.isDigit(arr[i]))) tag.append(arr[i]);
    		else if ((i>0)&&(arr[i]=='-')) tag.append('-');
    		else if ((i>0)&&(arr[i]=='.')) tag.append('.');
    		else if ((i>0)&&(arr[i]=='_')) tag.append('_');
    		else tag.append('_');
    	}

    	return tag.toString();
    }
   
}
