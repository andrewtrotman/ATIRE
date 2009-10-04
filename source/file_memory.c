/*
	FILE_MEMORY.C
	-------------
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "file_memory.h"

/*
	ANT_FILE_MEMORY::WRITE()
	------------------------
*/
long ANT_file_memory::write(unsigned char *data, long long size)
{
exit(printf("Cannot write to an in-memory file\n"));
#pragma warning (suppress: 4100)
}

/*
	ANT_FILE_MEMORY::PUTS()
	-----------------------
*/
long long ANT_file_memory::puts(char *string)
{
exit(printf("Cannot write to an in-memory file\n"));
#pragma warning (suppress: 4100)
}

/*
	ANT_FILE_MEMORY::OPEN()
	-----------------------
*/
long ANT_file_memory::open(char *filename, char *mode)
{
long result = 0;

file_position = 0;
if (ANT_file::open(filename, mode) != 0)
	{
	result = 1;
	length_of_file = ANT_file::file_length();
	if ((buffer = (unsigned char *)memory->malloc(length_of_file)) != NULL)
		{
		if (ANT_file::read(buffer, length_of_file) == 0)
			result = 0;
		ANT_file::close();
		}
	}
return result;
}

/*
	ANT_FILE_MEMORY::READ()
	-----------------------
*/
long ANT_file_memory::read(unsigned char *data, long long size)
{
memcpy(data, buffer + file_position, (size_t)size);
file_position += size;
return 1;		// success
}

/*
	ANT_FILE_MEMORY::SEEK()
	-----------------------
*/
void ANT_file_memory::seek(long long offset_from_start_of_file)
{
file_position = offset_from_start_of_file;
}
