/*
	PDF.C
	-----
	Original comes from http://www.codeproject.com/KB/cpp/ExtractPDFText.aspx
	That code states "This code is free. Use it for any purpose".  There are,
	of course, ANT modifications included here.

	Originally based on PDFReference15_v5.pdf from www.adobe.com
*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "zlib.h"
#include "../source/str.h"

//Keep this many previous recent characters for back reference:
#define oldchar 15

//Convert a recent set of characters into a number if there is one.
//Otherwise return -1:
/*
	EXTRACTNUMBER()
	---------------
	Walk backwards from the end of a string to the beginning of a possible number
	and convert that number into a float (or return -1 on error).
*/
float ExtractNumber(const char *search, int lastcharoffset)
{
char buffer[oldchar + 5];
float flt = -1.0;
int i = lastcharoffset;

while (i > 0 && search[i] == ' ')
	i--;

while (i > 0 && (isdigit(search[i]) || search[i] == '.'))
	i--;

memset(buffer, 0, sizeof(buffer));
strncpy(buffer, search + i + 1, lastcharoffset - i);

if (buffer[0] && sscanf(buffer, "%f", &flt))
	return flt;

return -1.0;
}

/*
	SEEN2()
	-------
//	Check if a certain 2 character token just came along (e.g. BT):
*/
bool seen2(const char *search, char *recent)
{
if (recent[oldchar - 3] == search[0] 
		&& recent[oldchar - 2] == search[1] 
		&& (recent[oldchar - 1] == ' ' || recent[oldchar - 1] == 0x0d || recent[oldchar - 1] == 0x0a) 
		&& (recent[oldchar - 4] == ' ' || recent[oldchar - 4] == 0x0d || recent[oldchar - 4] == 0x0a))
	return true;

return false;
}

/*
	PROCESSOUTPUT()
	---------------
*/
void ProcessOutput(FILE *file, char* output, size_t len)
{
bool intextobject = false;				//Are we currently inside a text object?
bool nextliteral = false;				//Is the next character literal (e.g. \\ to get a \ character or \( to get ( ):
int rbdepth = 0;						//() Bracket nesting level. Text appears inside ()
char oc[oldchar];						//Keep previous chars to get extract numbers etc.:
int j = 0;

for (j = 0; j < oldchar; j++)
	oc[j]=' ';

for (size_t i = 0; i < len; i++)
	{
	char c = output[i];

	if (intextobject)
		{
		if (rbdepth == 0)
			{
			if (seen2("TD", oc))			// Move text and set leading
				fputc('\n', file);
			else if (seen2("Td", oc))			// Move text
				fputc('\n', file);
			}
		if (rbdepth == 0)
			{
			if (seen2("ET", oc))			// End of a text object, also go to a new line.
				{
				intextobject = false;
				fputc('\n', file);
				}
			else if (c == '(' && !nextliteral) 
				{
				rbdepth = 1;			//Start outputting text!
				if (ExtractNumber(oc, oldchar - 1) > 100.0)
					fputc(' ', file);
				}
			}
		else if (rbdepth == 1)
			{
			if (c == ')' && !nextliteral) 
				rbdepth = 0;			//Stop outputting text
			else
				{
				//Just a normal text character:
				if (c == '\\' && !nextliteral)
					nextliteral = true;				//Only print out next character no matter what. Do not interpret.
				else
					{
					nextliteral = false;
					if (((c >= ' ') && (c <= '~')) || ((c >= 128) && (c < 255)))
						fputc(c, file);
					}
				}
			}
		}
//Store the recent characters for when we have to go back for a number:
	for (j = 0; j < oldchar - 1; j++)
		oc[j] = oc[j + 1];
	oc[oldchar - 1] = c;

	if (!intextobject)
		if (seen2("BT", oc))							// Start of a text object:
			intextobject = true;
	}
}

/*
	MAIN()
	------
*/
int main(int argc, char* argv[])
{
long filelen;
long morestreams;
char *stream_start, *stream_end;
char *disk_buffer, *buffer, *end;
FILE *fileo = stdout;
FILE *filei = fopen(argv[1], "rb");

if (filei && fileo)
	{
	/*
		Get the length of the file by seeking to the end then ftelling
		then go back to the start
	*/
	fseek(filei,0, SEEK_END);
	filelen = ftell(filei);
	fseek(filei,0, SEEK_SET);

	//Read the entire file into memory (!):
	buffer = disk_buffer = new char [filelen + 1];
	fread(buffer, filelen, 1 ,filei);  //must return 1
	end = buffer + filelen;
	buffer[filelen] = '\0';

	morestreams = true;

	//Now search the buffer repeated for streams of data:
	while (morestreams)
		{
		// Search for stream, endstream. We ought to first check the filter
		// of the object to make sure it if FlateDecode, but skip that for now!
		morestreams = false;
		if ((stream_start = memstr(buffer, "stream", end - buffer)) != NULL)
			if ((stream_end = memstr(stream_start + 6, "endstream", end - stream_start)) != NULL)
				{
				//Skip to beginning and end of the data stream:
				stream_start += 6;

				while (isspace(*stream_start))
					stream_start++;
				while (isspace(*(stream_end - 1)))
					stream_end--;

				//Assume output will fit into 10 times input buffer:
				uLongf outsize = (stream_end - stream_start) * 10;
				char *output = new char [outsize];

				if (uncompress((Bytef *)output, &outsize, (Bytef *)stream_start, stream_end - stream_start + 1) == Z_OK)
					ProcessOutput(fileo, output, outsize);
				delete [] output;

				buffer = stream_end + 7;
				morestreams = true;
				}
		}
	delete [] disk_buffer;
	fclose(filei);
	}

return 0;
}

