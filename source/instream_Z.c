/*
	INSTREAM_Z.C
	------------
*/
#include <string.h>
#include "instream_Z.h"
#include "memory.h"
#include "file.h"
#include "disk.h"
#include "../external/unencumbered/unlzw/unlzw.cpp"

/*
	ANT_INSTREAM_Z::ANT_INSTREAM_Z()
	--------------------------------
*/
ANT_instream_Z::ANT_instream_Z(ANT_memory *memory, char *filename) : ANT_instream(memory)
{
this->filename = (char *)memory->malloc(strlen(filename) + 1);
strcpy(this->filename, filename);
memory->realign();

binary = NULL;
text = NULL;

bytes_read = 0;
file_length = 0;
}

/*
	ANT_INSTREAM_Z::~ANT_INSTREAM_Z()
	---------------------------------
*/
ANT_instream_Z::~ANT_instream_Z()
{
delete [] binary;
delete [] text;
}

/*
	ANT_INSTREAM_Z::READ()
	----------------------
*/
long long ANT_instream_Z::read(unsigned char *buffer, long long bytes)
{
if (bytes == 0)		// read no bytes
	return 0;

if (binary == NULL)
	{
	long long disk_file_size;
	binary = (unsigned char *)ANT_disk::read_entire_file(filename, &disk_file_size);
	int error = unlzw(&text, &file_length, binary, disk_file_size);

	if (error != 0)
		{
		printf("filed to decode .Z file '%s'\n", filename);
		return -1;
		}
	else
		printf("Decoded %lld byte .Z into %zu bytes\n", disk_file_size, file_length);
	}

if (bytes_read >= file_length)
	return -1;		// at EOF so nothing to read

if (bytes_read + bytes > file_length)
	bytes = file_length - bytes_read;

memcpy(buffer, text + bytes_read, bytes);
bytes_read += bytes;

return bytes;
}
