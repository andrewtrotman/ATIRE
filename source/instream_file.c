/*
	INSTREAM_FILE.C
	---------------
*/
#include <string.h>
#include "instream_file.h"
#include "memory.h"
#include "file.h"

/*
	ANT_INSTREAM_FILE::ANT_INSTREAM_FILE()
	--------------------------------------
*/
ANT_instream_file::ANT_instream_file(ANT_memory *memory, char *filename) : ANT_instream(memory)
{
this->filename = (char *)memory->malloc(strlen(filename) + 1);
strcpy(this->filename, filename);
memory->realign();

file = NULL;
file_length = bytes_read = 0;
}

/*
	ANT_INSTREAM_FILE::~ANT_INSTREAM_FILE()
	---------------------------------------
*/
ANT_instream_file::~ANT_instream_file()
{
delete file;
}

/*
	ANT_INSTREAM_FILE::READ()
	-------------------------
*/
long long ANT_instream_file::read(unsigned char *buffer, long long bytes)
{
if (bytes == 0)		// read no bytes
	return 0;

if (file == NULL)
	{
	file = new ANT_file;
	if ((file->open(filename, "rb")) == 0)
		file_length = 0;
	else
		file_length = file->file_length();
	bytes_read = 0;
	}
if (bytes_read >= file_length)
	return -1;		// at EOF so nothing to read


if (bytes_read + bytes > file_length)
	bytes = file_length - bytes_read;

bytes_read += bytes;

file->read(buffer, bytes);
return bytes;
}

