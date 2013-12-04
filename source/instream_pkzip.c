/*
	INSTREAM_PKZIP.C
	----------------
*/
#include <string.h>
#include "directory_iterator_pkzip.h"
#include "instream_pkzip.h"
#include "memory.h"
#include "maths.h"
#include "file.h"

/*
	ANT_INSTREAM_PKZIP::ANT_INSTREAM_PKZIP()
	----------------------------------------
*/
ANT_instream_pkzip::ANT_instream_pkzip(ANT_memory *memory, char *filename) : ANT_instream(memory)
{
ANT_directory_iterator_object object;

this->filename = (char *)memory->malloc(strlen(filename) + 1);
strcpy(this->filename, filename);
memory->realign();
bytes_read = 0;
current_file_pointer = current_file = NULL;
current_file_length = 0;

source = new ANT_directory_iterator_pkzip(filename, 1);
if (source->first(&object) != NULL)
	{
	current_file_pointer = current_file = object.file;
	current_file_length = object.length;
	}
}

/*
	ANT_INSTREAM_PKZIP::~ANT_INSTREAM_PKZIP()
	-----------------------------------------
*/
ANT_instream_pkzip::~ANT_instream_pkzip()
{
delete [] current_file;
delete source;
}

/*
	ANT_INSTREAM_PKZIP::READ()
	--------------------------
*/
long long ANT_instream_pkzip::read(unsigned char *buffer, long long bytes)
{
long long allowed;

if (current_file == NULL)
	return -1;			// EOF

allowed = ANT_min(bytes, (long long)(current_file + current_file_length - current_file_pointer));
memcpy(buffer, current_file_pointer, (size_t)allowed);
current_file_pointer += allowed;
if (current_file_pointer > current_file + current_file_length)
	{
	delete [] current_file;
	current_file = NULL;
	}
bytes_read += allowed;

return allowed;
}

