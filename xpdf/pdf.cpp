//This file contains extremely crude C source code to extract plain text
//from a PDF file. It is only intended to show some of the basics involved
//in the process and by no means good enough for commercial use.
//But it can be easily modified to suit your purpose. Code is by no means
//warranted to be bug free or suitable for any purpose.
//
//Adobe has a web site that converts PDF files to text for free,
//so why would you need something like this? Several reasons:
//
//1) This code is entirely free including for commericcial use. It only
//   requires ZLIB (from www.zlib.org) which is entirely free as well.
//
//2) This code tries to put tabs into appropriate places in the text,
//   which means that if your PDF file contains mostly one large table,
//   you can easily take the output of this program and directly read it
//   into Excel! Otherwise if you select and copy the text and paste it into
//   Excel there is no way to extract the various columns again.
//
//This code assumes that the PDF file has text objects compressed
//using FlateDecode (which seems to be standard).
//
//This code is free. Use it for any purpose.
//The author assumes no liability whatsoever for the use of this code.
//Use it at your own risk!


//PDF file strings (based on PDFReference15_v5.pdf from www.adobve.com:
//
//BT = Beginning of a text object, ET = end of a text object
//5 Ts = superscript
//-5 Ts = subscript
//Td move to start next line

//No precompiled headers, but uncomment if need be:
#include "stdafx.h"

#include <stdio.h>
#include <windows.h>

//YOur project must also include zdll.lib (ZLIB) as a dependency.
//ZLIB can be freely downloaded from the internet, www.zlib.org
//Use 4 byte struct alignment in your project!

#include "zlib.h"

//Find a string in a buffer:
size_t FindStringInBuffer (char* buffer, char* search, size_t buffersize)
{
	char* buffer0 = buffer;

	size_t len = strlen(search);
	bool fnd = false;
	while (!fnd)
	{
		fnd = true;
		for (size_t i=0; i<len; i++)
		{
			if (buffer[i]!=search[i])
			{
				fnd = false;
				break;
			}
		}
		if (fnd) return buffer - buffer0;
		buffer = buffer + 1;
		if (buffer - buffer0 + len >= buffersize) return -1;
	}
	return -1;
}

//Keep this many previous recent characters for back reference:
#define oldchar 15

//Convert a recent set of characters into a number if there is one.
//Otherwise return -1:
float ExtractNumber(const char* search, int lastcharoffset)
{
	int i = lastcharoffset;
	while (i>0 && search[i]==' ') i--;
	while (i>0 && (isdigit(search[i]) || search[i]=='.')) i--;
	float flt=-1.0;
	char buffer[oldchar+5]; ZeroMemory(buffer,sizeof(buffer));
	strncpy(buffer, search+i+1, lastcharoffset-i);
	if (buffer[0] && sscanf(buffer, "%f", &flt))
	{
		return flt;
	}
	return -1.0;
}

//Check if a certain 2 character token just came along (e.g. BT):
bool seen2(const char* search, char* recent)
{
if (    recent[oldchar-3]==search[0] 
     && recent[oldchar-2]==search[1] 
	 && (recent[oldchar-1]==' ' || recent[oldchar-1]==0x0d || recent[oldchar-1]==0x0a) 
	 && (recent[oldchar-4]==' ' || recent[oldchar-4]==0x0d || recent[oldchar-4]==0x0a)
	 )
	{
		return true;
	}
	return false;
}

//This method processes an uncompressed Adobe (text) object and extracts text.
void ProcessOutput(FILE* file, char* output, size_t len)
{
	//Are we currently inside a text object?
	bool intextobject = false;

	//Is the next character literal (e.g. \\ to get a \ character or \( to get ( ):
	bool nextliteral = false;
	
	//() Bracket nesting level. Text appears inside ()
	int rbdepth = 0;

	//Keep previous chars to get extract numbers etc.:
	char oc[oldchar];
	int j=0;
	for (j=0; j<oldchar; j++) oc[j]=' ';

	for (size_t i=0; i<len; i++)
	{
		char c = output[i];
		if (intextobject)
		{
			if (rbdepth==0 && seen2("TD", oc))
			{
				//Positioning.
				//See if a new line has to start or just a tab:
				float num = ExtractNumber(oc,oldchar-5);
				if (num>1.0)
				{
					fputc(0x0d, file);
					fputc(0x0a, file);
				}
				if (num<1.0)
				{
					fputc('\t', file);
				}
			}
			if (rbdepth==0 && seen2("ET", oc))
			{
				//End of a text object, also go to a new line.
				intextobject = false;
				fputc(0x0d, file);
				fputc(0x0a, file);
			}
			else if (c=='(' && rbdepth==0 && !nextliteral) 
			{
				//Start outputting text!
				rbdepth=1;
				//See if a space or tab (>1000) is called for by looking
				//at the number in front of (
				int num = ExtractNumber(oc,oldchar-1);
				if (num>0)
				{
					if (num>1000.0)
					{
						fputc('\t', file);
					}
					else if (num>100.0)
					{
						fputc(' ', file);
					}
				}
			}
			else if (c==')' && rbdepth==1 && !nextliteral) 
			{
				//Stop outputting text
				rbdepth=0;
			}
			else if (rbdepth==1) 
			{
				//Just a normal text character:
				if (c=='\\' && !nextliteral)
				{
					//Only print out next character no matter what. Do not interpret.
					nextliteral = true;
				}
				else
				{
					nextliteral = false;
					if ( ((c>=' ') && (c<='~')) || ((c>=128) && (c<255)) )
					{
						fputc(c, file);
					}
				}
			}
		}
		//Store the recent characters for when we have to go back for a number:
		for (j=0; j<oldchar-1; j++) oc[j]=oc[j+1];
		oc[oldchar-1]=c;
		if (!intextobject)
		{
			if (seen2("BT", oc))
			{
				//Start of a text object:
				intextobject = true;
			}
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	//Discard existing output:
	FILE* fileo = fopen("c:\\pdf\\output2.txt", "w");
	if (fileo) fclose(fileo);
	fileo = fopen("c:\\pdf\\output2.txt", "a");

	//Open the PDF source file:
	FILE* filei = fopen("c:\\pdf\\somepdf.pdf", "rb");

	if (filei && fileo)
	{
		//Get the file length:
		int fseekres = fseek(filei,0, SEEK_END);   //fseek==0 if ok
		long filelen = ftell(filei);
		fseekres = fseek(filei,0, SEEK_SET);

		//Read ethe ntire file into memory (!):
		char* buffer = new char [filelen]; ZeroMemory(buffer, filelen);
		size_t actualread = fread(buffer, filelen, 1 ,filei);  //must return 1

		bool morestreams = true;

		//Now search the buffer repeated for streams of data:
		while (morestreams)
		{
			//Search for stream, endstream. We ought to first check the filter
			//of the object to make sure it if FlateDecode, but skip that for now!
			size_t streamstart = FindStringInBuffer (buffer, "stream", filelen);
			size_t streamend   = FindStringInBuffer (buffer, "endstream", filelen);
			if (streamstart>0 && streamend>streamstart)
			{
				//Skip to beginning and end of the data stream:
				streamstart += 6;

				if (buffer[streamstart]==0x0d && buffer[streamstart+1]==0x0a) streamstart+=2;
				else if (buffer[streamstart]==0x0a) streamstart++;

				if (buffer[streamend-2]==0x0d && buffer[streamend-1]==0x0a) streamend-=2;
				else if (buffer[streamend-1]==0x0a) streamend--;

				//Assume output will fit into 10 times input buffer:
				size_t outsize = (streamend - streamstart)*10;
				char* output = new char [outsize]; ZeroMemory(output, outsize);

				//Now use zlib to inflate:
				z_stream zstrm; ZeroMemory(&zstrm, sizeof(zstrm));

				zstrm.avail_in = streamend - streamstart + 1;
				zstrm.avail_out = outsize;
				zstrm.next_in = (Bytef*)(buffer + streamstart);
				zstrm.next_out = (Bytef*)output;

				int rsti = inflateInit(&zstrm);
				if (rsti == Z_OK)
				{
					int rst2 = inflate (&zstrm, Z_FINISH);
					if (rst2 >= 0)
					{
						//Ok, got something, extract the text:
						size_t totout = zstrm.total_out;
						ProcessOutput(fileo, output, totout);
					}
				}
				delete[] output; output=0;
				buffer+= streamend + 7;
				filelen = filelen - (streamend+7);
			}
			else
			{
				morestreams = false;
			}
		}
		fclose(filei);
	}
	if (fileo) fclose(fileo);
	return 0;
}

