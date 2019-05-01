/*
	INSTREAM_TGZ.C
	----------------
*/
#include <string.h>
#include "directory_iterator_tar.h"
#include "instream_tgz.h"
#include "instream_deflate.h"
#include "instream_file.h"
#include "instream_buffer.h"
#include "memory.h"
#include "maths.h"
#include "file.h"

/*
	ANT_INSTREAM_TGZ::ANT_INSTREAM_TGZ()
	------------------------------------
*/
ANT_instream_tgz::ANT_instream_tgz(ANT_memory *memory, char *filename) : ANT_instream(memory)
{
ANT_directory_iterator_object object;

this->filename = (char *)memory->malloc(strlen(filename) + 1);
strcpy(this->filename, filename);
memory->realign();
bytes_read = 0;
current_file_pointer = current_file = NULL;
current_file_length = 0;

deflater = new ANT_instream_deflate(memory, new ANT_instream_file(memory, filename));
ANT_instream *instream_buffer = new ANT_instream_buffer(memory, deflater);

source = new ANT_directory_iterator_tar(instream_buffer, ANT_directory_iterator::READ_FILE);
if (source->first(&object) != NULL)
	{
	current_file_pointer = current_file = object.file;
	current_file_length = object.length;
	}
}

/*
	ANT_INSTREAM_TGZ::~ANT_INSTREAM_TGZ()
	-------------------------------------
*/
ANT_instream_tgz::~ANT_instream_tgz()
{
delete source;
}

/*
	ANT_INSTREAM_TGZ::READ()
	------------------------
*/
long long ANT_instream_tgz::read(unsigned char *buffer, long long bytes)
{
long long allowed;

if (current_file == NULL)
	{
	ANT_directory_iterator_object object;

	if (source->next(&object) == NULL)
		return -1;			// at EOF

	current_file_pointer = current_file = object.file;
	current_file_length = object.length;
	}

allowed = ANT_min(bytes, (long long)(current_file + current_file_length - current_file_pointer));
memcpy(buffer, current_file_pointer, (size_t)allowed);
current_file_pointer += allowed;
if (current_file_pointer >= current_file + current_file_length)
	{
	delete [] current_file;
	current_file = NULL;
	}
bytes_read += allowed;

return allowed;
}

