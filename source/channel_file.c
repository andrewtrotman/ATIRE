/*
	CHANNEL_FILE.C
	--------------
*/
#include "channel_file.h"
#include "str.h"

/*
	ANT_CHANNEL_FILE::ANT_CHANNEL_FILE()
	------------------------------------
	if filename == NULL then use stdin and stout else use the named file
*/
ANT_channel_file::ANT_channel_file(char *filename)
{
if (filename == NULL)
	this->filename = NULL;
else
	this->filename = strnew(filename);

infile = stdin;
outfile = stdout;
}

/*
	ANT_CHANNEL_FILE::~ANT_CHANNEL_FILE()
	-------------------------------------
*/
ANT_channel_file::~ANT_channel_file()
{
/*
	outfile and infile are the same FILE * except in the case of stdio
	when they are the same.  We don't close stdio and stdout but we do
	close all other files.  Since outfile and infile are the same we only
	check one and close that.
*/
if (outfile != stdout)
	fclose(outfile);

delete [] filename;
}

/*
	ANT_CHANNEL_FILE::CONNECT()
	---------------------------
*/
void ANT_channel_file::connect(void)
{
if (filename != NULL && infile == stdin)
	{
	infile = fopen(filename, "a+b");		// open for append
	fseek(infile, 0L, SEEK_SET);
	outfile = infile;
	}
}

/*
	ANT_CHANNEL_FILE::BLOCK_WRITE()
	-------------------------------
*/
long long ANT_channel_file::block_write(char *source, long long length)
{
connect();
return fwrite(source, (size_t)length, 1, outfile);
}

/*
	ANT_CHANNEL_FILE::BLOCK_READ()
	------------------------------
*/
char *ANT_channel_file::block_read(char *into, long long length)
{
connect();
if (feof(infile))
	return NULL;

return fread(into, (size_t)length, 1, infile) == 1 ? into : NULL;

}

/*
	ANT_CHANNEL_FILE::GETSZ()
	-------------------------
*/
char *ANT_channel_file::getsz(char terminator)
{
char *buffer = NULL;
long buffer_length, used, old_length, block_size = 1024;
long next;

buffer_length = used = 0;
connect();

/*
	At EOF and so we fail
*/
if (feof(infile))
	return NULL;

buffer = new char [old_length = block_size + 2];
/*
	Else we do a gets() and stop when we hit the terminator
*/
while ((next = fgetc(infile)) != terminator)
	{
	if  (next == EOF)
		if (used == 0)
			{
			delete [] buffer;
			return NULL;
			}
		else
			break;
		
	if (used >= buffer_length)
		{
		old_length = buffer_length;
		buffer_length += block_size;
		buffer = strrenew(buffer, old_length, buffer_length + 2);		// +1 for terminator and +1 for the '\0' on the end
		}
	buffer[used] = (char)next;
	used++;
	}

if (buffer != NULL)
	{
	/*
		We got line of stuff so terminate it with the terminator then null terminate that.
	*/
	buffer[used] = (char)next;
	buffer[used + 1] = '\0';
	}

return buffer;
}

