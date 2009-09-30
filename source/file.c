/*
	FILE.C
	------
*/
#include <string.h>
#include "file.h"
#include "memory.h"
#include "file_internals.h"

#ifdef linux
	#include <sys/stat.h>
#elif defined(__APPLE__)
	#include <sys/stat.h>
#elif defined(_MSC_VER)
	#include <windows.h>
#else
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
#endif

/*
	ANT_FILE::ANT_FILE()
	--------------------
*/
ANT_file::ANT_file(ANT_memory *memory)
{
internals = new ANT_file_internals;
this->memory = memory;
buffer = NULL;
buffer_size = 0;
buffer_used = 0;
file_position = 0;
setvbuff(1024 * 1024);		// use a 1MB buffer by default.
bytes_written = bytes_read = 0;
}

/*
	ANT_FILE::~ANT_FILE()
	---------------------
*/
ANT_file::~ANT_file()
{
close();
delete internals;
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
#ifdef _MSC_VER
	DWORD access_mode, creation_mode;
	char *ch;

	access_mode = creation_mode = 0;
	for (ch = mode; *ch != NULL; ch++)
		switch (*ch)
			{
			case 'r':
				access_mode |= GENERIC_READ;
				creation_mode |= OPEN_EXISTING;
				break;
			case 'w':
				access_mode |= GENERIC_WRITE;
				creation_mode |= CREATE_ALWAYS;
				break;
			case 'a':
				access_mode |= GENERIC_WRITE;
				creation_mode = OPEN_ALWAYS;
				break;
			case '+':
				access_mode |= GENERIC_READ | GENERIC_WRITE;
				break;
			}
	if ((internals->fp = CreateFile(filename, access_mode, FILE_SHARE_READ, NULL, creation_mode, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL)) == INVALID_HANDLE_VALUE)
		return 0;
#else
	if ((internals->fp = fopen(filename, mode)) == NULL)
		return 0;
#endif

file_position = 0;
if (strchr(mode, 'a') != NULL)
	seek(file_length());

return 1;
}

/*
	ANT_FILE::CLOSE()
	-----------------
*/
long ANT_file::close(void)
{
if (internals->fp != NULL)
	{
	flush();
	#ifdef _MSC_VER
		CloseHandle(internals->fp);
		internals->fp = INVALID_HANDLE_VALUE;
	#else
		fclose(internals->fp);
		internals->fp = NULL;
	#endif
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
	internals->write_file_64(internals->fp, buffer, buffer_used);
	buffer_used = 0;
	}
}

/*
	ANT_FILE::PUTS()
	----------------
*/
long long ANT_file::puts(char *string)
{
long long len;

write((unsigned char *)string, len = strlen(string));
write((unsigned char *)"\n", 1);

return len + 1;
}

/*
	ANT_FILE::WRITE()
	-----------------
*/
long ANT_file::write(unsigned char *data, long long size)
{
unsigned char *from;
long long block_size;

file_position += size;
if (buffer_used + size < buffer_size)
	{
	/*
		The data fits in the internal buffers
	*/
	memcpy(buffer + buffer_used, data, (size_t)size);
	buffer_used += size;
	}
else
	{
	/*
		The data does not fit in the internal buffers so it is
		necessary to flush the buffers and then do the write
	*/
	from = data;
	block_size = size <= buffer_size ? size : buffer_size;
	do
		{
		flush();
		memcpy(buffer, from,(size_t)block_size);
		buffer_used += block_size;
		from += block_size;
		size -= block_size;
		}
	while (size > 0);
	}

/*
	Keep track of the total number of bytes we've been asked to write to the file
*/
bytes_written += size;

return 1;
}

/*
	ANT_FILE::READ()
	----------------
*/
long ANT_file::read(unsigned char *data, long long size)
{
/*
	Keep track of the number of bytes we've been asked to write
*/
bytes_read += size;

/*
	Flush the write cache
*/
flush();
/*
	Take note of the new file position (at the end of the read)
*/
file_position += size;		// this is where we'll be at the end of the read

/*
	And now perform the read
*/
return internals->read_file_64(internals->fp, data, size);
}

/*
	ANT_FILE::SEEK()
	----------------
*/
void ANT_file::seek(long long offset_from_start_of_file)
{
#ifdef _MSC_VER
	LARGE_INTEGER offset;
#endif

/*
	Empty the write buffer
*/
flush();

/*
	Now do the seek
*/
#ifdef _MSC_VER
	offset.QuadPart = offset_from_start_of_file;
	SetFilePointerEx(internals->fp, offset, NULL, FILE_BEGIN);
#else
	fseek(internals->fp, offset_from_start_of_file, SEEK_SET);
#endif

/*
	Store the file file position
*/
file_position = offset_from_start_of_file;
}

/*
	ANT_FILE::FILE_LENGTH()
	-----------------------
*/
long long ANT_file::file_length(void)
{
#ifdef _MSC_VER
	LARGE_INTEGER ans;
#else
	struct stat details;
#endif

#ifdef _MSC_VER
	if (internals->fp != INVALID_HANDLE_VALUE)
		if (GetFileSizeEx(internals->fp, &ans) != 0)
			return ans.QuadPart;
#else
	if (internals->fp != NULL)
		if (fstat(fileno(internals->fp), &details) == 0)
			return details.st_size;
#endif

return 0;
}
