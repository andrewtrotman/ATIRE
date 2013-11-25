/*
	INSTREAM_MEMORY.C
	-----------------
*/
#include <string.h>
#include "instream_memory.h"

/*
	ANT_INSTREAM_MEMORY::ANT_INSTREAM_MEMORY()
	------------------------------------------
*/
ANT_instream_memory::ANT_instream_memory(char *infile, long long length) : ANT_instream(NULL)
{
file = infile;
file_length = length;
bytes_read = 0;

}

/*
	ANT_INSTREAM_MEMORY::READ()
	---------------------------
*/
long long ANT_instream_memory::read(unsigned char *buffer, long long bytes)
{
long long bytes_to_copy;

if (bytes_read > file_length)
	return -1;
else if (bytes_read + bytes < file_length)
	bytes_to_copy = bytes;
else
	bytes_to_copy = file_length - bytes_read;

memmove(buffer, file + bytes_read, (size_t)bytes_to_copy);

bytes_read += bytes;

return bytes_to_copy;
}
