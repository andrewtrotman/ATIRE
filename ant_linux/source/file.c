/*
	FILE.C
	------
*/
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
/* #include <io.h> */
#include "file.h"
#include "memory.h"

/*
	ANT_FILE::ANT_FILE()
	--------------------
*/
ANT_file::ANT_file(ANT_memory *memory)
{
this->memory = memory;
buffer = NULL;
buffer_size = 0;
buffer_used = 0;
file_position = 0;
}

/*
	ANT_FILE::~ANT_FILE()
	---------------------
*/
ANT_file::~ANT_file()
{
}

/*
	ANT_FILE::SETVBUFF()
	--------------------
*/
long ANT_file::setvbuff(long size)
{
buffer_size = size;
buffer = (unsigned char *)memory->malloc(size);

return buffer == NULL ? 0 : 1;
}

/*
	ANT_FILE::OPEN()
	----------------
*/
long ANT_file::open(const char *filename, const char *mode)
{
if ((fp = fopen(filename, mode)) == NULL)
	return 0;

file_position = 0;

return 1;
}

/*
	ANT_FILE::CLOSE()
	-----------------
*/
long ANT_file::close(void)
{
flush();
fclose(fp);
fp = NULL;
return 1;
}

/*
	ANT_FILE::FLUSH()
	-----------------
*/
void ANT_file::flush(void)
{
if (buffer_used > 0)
	{
	fwrite(buffer, buffer_used, 1, fp);
	buffer_used = 0;
	}
}

/*
	ANT_FILE::WRITE()
	-----------------
*/
long ANT_file::write(unsigned char *data, long size)
{
unsigned char *from;
long block_size;

file_position += size;
if (buffer_used + size < buffer_size)
	{
	memcpy(buffer + buffer_used, data, size);
	buffer_used += size;
	}
else
	{
	from = data;
	block_size = size <= buffer_size ? size : buffer_size;
	do
		{
		flush();
		memcpy(buffer, from, block_size);
		buffer_used += block_size;
		from += block_size;
		size -= block_size;
		}
	while (size > 0);
	}

return 1;
}

/*
	CHAR *ANT_FILE::READ()
	----------------------
*/
long ANT_file::read(unsigned char *data, long size)
{
flush();
file_position += size;
return fread(data, size, 1, fp);
}

/*
	ANT_FILE::TELL()
	----------------
*/
long long ANT_file::tell(void)
{
return file_position;
}

/*
	ANT_FILE::SEEK()
	----------------
*/
void ANT_file::seek(long long offset_from_start_of_file)
{
flush();
fseek(fp, offset_from_start_of_file, SEEK_SET);
file_position = offset_from_start_of_file;
}

/*
	ANT_FILE::FILE_LENGTH()
	-----------------------
*/
long long ANT_file::file_length(void)
{
if (fp == NULL)
	return 0;
 else {
     struct stat buff;
     fstat(fileno(fp), &buff);
     return buff.st_size;
 }
//	return (long long int) filelength(fileno(fp));
}

