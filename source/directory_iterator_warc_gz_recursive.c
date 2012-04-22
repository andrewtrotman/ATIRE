/*
	DIRECTORY_ITERATOR_WARC_GZ_RECURSIVE.C
	--------------------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "directory_iterator_warc_gz_recursive.h"
#include "memory.h"
#include "instream_file.h"
#include "instream_deflate.h"
#include "instream_buffer.h"
#include "directory_iterator_recursive.h"
#include "directory_iterator_warc.h"

/*
	ANT_DIRECTORY_ITERATOR_WARC_GZ_RECURSIVE::ANT_DIRECTORY_ITERATOR_WARC_GZ_RECURSIVE()
	------------------------------------------------------------------------------------
*/
ANT_directory_iterator_warc_gz_recursive::ANT_directory_iterator_warc_gz_recursive(char *source, long get_file, long long scrubbing_options) : ANT_directory_iterator("", get_file)
{
ANT_directory_iterator_object filename;

this->source = source;
filename_provider = new ANT_directory_iterator_recursive(source, 0);

this->scrubbing_options = scrubbing_options;

more_files = filename_provider->first(&filename);
first_time = true;

file_stream = NULL;
decompressor = NULL;
instream_buffer = NULL;
dewarcer = NULL;
memory = NULL;

new_provider(filename.filename);
}

/*
	ANT_DIRECTORY_ITERATOR_WARC_GZ_RECURSIVE::~ANT_DIRECTORY_ITERATOR_WARC_GZ_RECURSIVE()
	-------------------------------------------------------------------------------------
*/
ANT_directory_iterator_warc_gz_recursive::~ANT_directory_iterator_warc_gz_recursive()
{
delete file_stream;
delete decompressor;
delete instream_buffer;
delete dewarcer;
delete memory;
}

/*
	ANT_DIRECTORY_ITERATOR_WARC_GZ_RECURSIVE::NEW_PROVIDER()
	--------------------------------------------------------
*/
ANT_directory_iterator_warc *ANT_directory_iterator_warc_gz_recursive::new_provider(char *filename)
{
delete file_stream;
delete decompressor;
delete instream_buffer;
delete dewarcer;
delete memory;

memory = new ANT_memory(1024 * 1024);
file_stream = new ANT_instream_file(memory, filename);
decompressor = new ANT_instream_deflate(memory, file_stream);
instream_buffer = new ANT_instream_buffer(memory, decompressor);
dewarcer = new ANT_directory_iterator_warc(instream_buffer, ANT_directory_iterator::READ_FILE);

return dewarcer;
}

/*
	ANT_DIRECTORY_ITERATOR_WARC_GZ_RECURSIVE::NEXT()
	------------------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_warc_gz_recursive::next(ANT_directory_iterator_object *object)
{
ANT_directory_iterator_object *got;

while (more_files != NULL)
	{
	if (first_time)
		got = dewarcer->first(object);
	else
		got = dewarcer->next(object);

	first_time = false;

	if (got == NULL)
		{
		if ((more_files = filename_provider->next(object)) != NULL)
			{
			new_provider(object->filename);
			first_time = true;
			}
		}
	else
		return got;
		
	}

return NULL;
}
