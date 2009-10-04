/*
	STREAM_TAR.C
	------------
*/
#include <stdio.h>
#include <stdlib.h>
#include "stream_tar.h"

/*
	ANT_STREAM_TAR::FILENAME()
	--------------------------
*/
char *ANT_stream_tar::filename(void)
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
	ANT_STREAM_TAR::DETOX()
	-----------------------
*/
inline long ANT_stream_tar::detox(char *octal)
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
	ANT_STREAM_TAR::FIRST()
	-----------------------
*/
long long ANT_stream_tar::first(char *filename)
{
bytes_read = 0;
if (file_open(filename))
	if (file_read((unsigned char *)&header, TAR_BLOCK_SIZE))
		return length_of_file_in_bytes = detox(header.filesize_in_bytes);
	else
		file_close();

return -1;
}

/*
	ANT_STREAM_TAR::NEXT()
	----------------------
*/
long long ANT_stream_tar::next(void)
{
long long extras;

/*
	If we're not at the end of the block then read one block at a time until we are.
*/
while (length_of_file_in_bytes - bytes_read > TAR_BLOCK_SIZE)
	file_read(buffer, TAR_BLOCK_SIZE);				// read to end of file

/*
	Now read up to the end of the  current block
*/
if ((extras = bytes_read % TAR_BLOCK_SIZE) != 0)
	file_read(buffer, TAR_BLOCK_SIZE - extras);

/*
	We've read no bytes from the current file
*/
bytes_read = 0;

/*
	Read the header of the next file
*/
if (file_read((unsigned char *)&header, TAR_BLOCK_SIZE))
	{
	if (header.filename[0] == '\0')
		{
		file_close();
		return -1;																// at end of file
		}
	else
		return length_of_file_in_bytes = detox(header.filesize_in_bytes);		// return the length
	}

/*
	Read has failed so we are probably at EOF (and the TAR file is broken)
*/
file_close();
return -2;
}

/*
	ANT_STREAM_TAR::FILE_OPEN()
	---------------------------
*/
long ANT_stream_tar::file_open(char *filename)
{
return (fp = fopen(filename, "rb")) != NULL;
}

/*
	ANT_STREAM_TAR::FILE_CLOSE()
	----------------------------
*/
long ANT_stream_tar::file_close(void)
{
fclose(fp);
return 1;
}

/*
	ANT_STREAM_TAR::FILE_READ()
	---------------------------
*/
unsigned char *ANT_stream_tar::file_read(unsigned char *buffer, long long size)
{
if (fread(buffer, (size_t)size, 1, fp))
	return buffer;
else
	return NULL;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_stream_tar tar_file;
long long size;

if (argc != 2)
	exit(printf("usage:%s <infile.tar>\n", argv[0]));

for (size = tar_file.first(argv[1]); size >= 0; size = tar_file.next())
	{
	puts(tar_file.filename());
	unsigned char *buffer = new unsigned char [(size_t)size];
	tar_file.read(buffer, size);
	delete buffer;
	}

return 0;
}
