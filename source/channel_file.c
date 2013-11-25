/*
	CHANNEL_FILE.C
	--------------
*/
#include "channel_file.h"
#include "str.h"
#include "memory.h"
#include "instream_file.h"
#include "instream_file_star.h"
#include "instream_deflate.h"

/*
	ANT_CHANNEL_FILE::ANT_CHANNEL_FILE()
	------------------------------------
	if filename == NULL then use stdin and stout else use the named file
*/
ANT_channel_file::ANT_channel_file(char *filename)
{
memory = NULL;
if (filename == NULL)
	this->filename = NULL;
else
	this->filename = strnew(filename);

if (filename == NULL)
	{
	outfile = stdout;
	infile = new ANT_instream_file_star(stdin);
	eof = false;
	}
else
	{
	outfile = NULL;
	infile = NULL;
	eof = true;
	}
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
if (outfile != stdout && outfile != NULL)
	fclose(outfile);

delete [] filename;
delete infile;
delete memory;
}

/*
	ANT_CHANNEL_FILE::BLOCK_WRITE()
	-------------------------------
*/
long long ANT_channel_file::block_write(char *source, long long length)
{
if (outfile == NULL)
	{
	outfile = fopen(filename, "a+b");		// open for append
	fseek(outfile, 0L, SEEK_SET);
	}
return fwrite(source, (size_t)length, 1, outfile);
}

/*
	ANT_CHANNEL_FILE::BLOCK_READ()
	------------------------------
*/
char *ANT_channel_file::block_read(char *into, long long length)
{
long filename_length;

if (infile == NULL)
	{
	memory = new ANT_memory(1024 * 1024);		// use a 1MB buffer;

	filename_length = strlen(filename);
	if (filename_length > 3 && strcmp(filename + filename_length - 3, ".gz") == 0)
		infile = new ANT_instream_deflate(memory, new ANT_instream_file(memory, filename));
	else
		infile = new ANT_instream_file(memory, filename);
	eof = false;
	}

if (eof)
	return NULL;

if (infile->read(into, (size_t)length) == 1)
	return into;

eof = true;
return NULL;
}

/*
	ANT_CHANNEL_FILE::GETSZ()
	-------------------------
*/
char *ANT_channel_file::getsz(char terminator)
{
char *buffer = NULL;
long bytes_read, buffer_length, used, old_length, block_size = 1024;
char next, *got;

bytes_read = buffer_length = used = 0;

buffer = new char [old_length = block_size + 2];
*buffer = '\0';
/*
	Else we do a gets() and stop when we hit the terminator
*/
while ((got = block_read(&next, 1)) != NULL)
	{
	bytes_read++;
	if (next == terminator)
		break;
	if  (got == NULL)
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

if (bytes_read == 0)
	{
	delete [] buffer;
	return NULL;
	}

return buffer;
}

