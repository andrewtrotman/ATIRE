/*
	DIRECTORY_ITERATOR_TAR.C
	------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <new>
#include "pragma.h"
#include "directory_iterator_tar.h"

/*
	ANT_DIRECTORY_ITERATOR_TAR::FILENAME()
	--------------------------------------
*/
char *ANT_directory_iterator_tar::filename(void)
{
if (header.filename_prefix[0] != '\0')
	{
	sprintf((char *)buffer, "%s/%s", header.filename_prefix, header.filename);
	return (char *)buffer;
	}
else
	return header.filename;
}

/*
	ANT_DIRECTORY_ITERATOR_TAR::DETOX()
	-----------------------------------
*/
inline long ANT_directory_iterator_tar::detox(char *octal)
{
long result;

result = *octal++ - '0';
result = result * 8 + (*octal++ - '0');
result = result * 8 + (*octal++ - '0');
result = result * 8 + (*octal++ - '0');
result = result * 8 + (*octal++ - '0');
result = result * 8 + (*octal++ - '0');
result = result * 8 + (*octal++ - '0');
result = result * 8 + (*octal++ - '0');
result = result * 8 + (*octal++ - '0');
result = result * 8 + (*octal++ - '0');
return result * 8 + (*octal++ - '0');
}

/*
	ANT_DIRECTORY_ITERATOR_TAR::FIRST()
	-----------------------------------
*/
char *ANT_directory_iterator_tar::first(char *wildcard)
{
bytes_read = 0;
if (source->read((unsigned char *)&header, TAR_BLOCK_SIZE) == TAR_BLOCK_SIZE)
	{
	length_of_file_in_bytes = detox(header.filesize_in_bytes);
	if (header.file_type == '0' || header.file_type == 0)
		return filename();		// we're a file
	else
		return next();			// we're not a file so go and get one
	}

return NULL;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_DIRECTORY_ITERATOR_TAR::NEXT()
	----------------------------------
*/
char *ANT_directory_iterator_tar::next(void)
{
long long extras;

/*
	read up to the end of the current block
*/
if (bytes_read % TAR_BLOCK_SIZE != 0)
	if ((extras = TAR_BLOCK_SIZE - (bytes_read % TAR_BLOCK_SIZE)) != 0)
		source->read(buffer, extras);

/*
	If we're not at the end of the file then read one block at a time until we are.
*/
while (bytes_read < length_of_file_in_bytes)
	{
	source->read(buffer, TAR_BLOCK_SIZE);				// read to end of file
	bytes_read += TAR_BLOCK_SIZE;
	}

/*
	We've read no bytes from the current file
*/
bytes_read = 0;

/*
	Read the header of the next file
*/
if (source->read((unsigned char *)&header, TAR_BLOCK_SIZE) == TAR_BLOCK_SIZE)
	{
	if (header.filename[0] == '\0')
		return NULL;		// at end of archive (kinda-sorta, there should be two of these)

	length_of_file_in_bytes = detox(header.filesize_in_bytes);
	if (header.file_type == '0' || header.file_type == '\0')
		return filename();		// we're a file
	else
		return next();			// we're not a file so go and get one
	}

/*
	Read has failed so we are probably at EOF (and the TAR file is broken)
*/
return NULL;
}

/*
	ANT_DIRECTORY_ITERATOR_TAR::READ_ENTIRE_FILE()
	----------------------------------------------
*/
char *ANT_directory_iterator_tar::read_entire_file(long long *length)
{
unsigned char *buffer;

buffer = new (std::nothrow) unsigned char [length_of_file_in_bytes + 1];
source->read(buffer, length_of_file_in_bytes);
buffer[length_of_file_in_bytes] = '\0';			// NULL terminate the contents of the file.
*length = bytes_read = length_of_file_in_bytes;

return (char *)buffer;
}

