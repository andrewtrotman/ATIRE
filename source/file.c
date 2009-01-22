/*
	FILE.C
	------
*/
#include <string.h>
#include "file.h"
#include "memory.h"

#ifdef linux
	#include <sys/stat.h>
#ifndef _LARGEFILE_SOURCE
	#define _LARGEFILE_SOURCE
#endif
#ifndef _LARGEFILE64_SOURCE
	#define _LARGEFILE64_SOURCE
#endif
	#define FILE_OFFSET_BITS 64
	#define ftell ftello
	#define fseek fseeko
	#define fstat fstat64
	#define stat stat64
#elif defined(__APPLE__)
	#include <sys/stat.h>
#elif defined(_MSC_VER)
	#include <sys/types.h>
	#include <sys/stat.h>

	#define fseek _fseeki64
	#define fileno _fileno
	#define stat _stat64
	#define fstat _fstat64
#endif


/*
	ANT_FILE::ANT_FILE()
	--------------------
*/
ANT_file::ANT_file(ANT_memory *memory)
{
fp = NULL;
this->memory = memory;
buffer = NULL;
buffer_size = 0;
buffer_used = 0;
file_position = 0;
setvbuff(1024 * 1024);		// use a 1MB buffer by default.
}

/*
	ANT_FILE::~ANT_FILE()
	---------------------
*/
ANT_file::~ANT_file()
{
close();
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
long ANT_file::open(char *filename, char *mode)
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
if (fp != NULL)
	{
	flush();
	fclose(fp);
	fp = NULL;
	}
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
	ANT_FILE::PUTS()
	----------------
*/
long ANT_file::puts(char *string)
{
long len;

write((unsigned char *)string, len = strlen(string));
write((unsigned char *)"\n", 1);

return len + 1;
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
	ANT_FILE::READ()
	----------------
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
struct stat details;
long ans;

if (fp != NULL)
	{
	ans = fstat(fileno(fp), &details);
	return details.st_size;
	}

return 0;
}

