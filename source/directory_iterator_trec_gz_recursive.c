/*
	 DIRECTORY_ITERATOR_TREC_GZ_RECURSIVE.C
	 --------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "directory_iterator_trec_gz_recursive.h"
#include "directory_iterator_scrub.h"
#include "memory.h"
#include "instream_file.h"
#include "instream_deflate.h"
#include "instream_scrub.h"
#include "instream_buffer.h"
#include "directory_iterator_recursive.h"
#include "directory_iterator_file_buffered.h"

long ANT_directory_iterator_trec_gz_recursive::tid = 0;

/*
	ANT_DIRECTORY_ITERATOR_TREC_GZ_RECURSIVE::ANT_DIRECTORY_ITERATOR_TREC_GZ_RECURSIVE()
	------------------------------------------------------------------------------------
*/
ANT_directory_iterator_trec_gz_recursive::ANT_directory_iterator_trec_gz_recursive(char *source, long get_file, long long scrubbing_options) : ANT_directory_iterator("", get_file)
{
ANT_directory_iterator_object filename;

this->source = source;
filename_provider = new ANT_directory_iterator_recursive(source, 0);

this->scrubbing_options = scrubbing_options;

more_files = filename_provider->first(&filename);
first_time = true;

file_stream = NULL;
decompressor = NULL;
instream_buffer_a = instream_buffer_b = NULL;
scrubber = NULL;
detrecer = NULL;
memory = NULL;

new_provider(filename.filename);

wait_input_time = 0;
wait_output_time = 0;
process_time = 0;
clock = new ANT_stats(new ANT_memory);

message = new char[50];
sprintf(message, "trec_gz_recursive %ld ", ANT_directory_iterator_trec_gz_recursive::tid++);
}

/*
	ANT_DIRECTORY_ITERATOR_TREC_GZ_RECURSIVE::~ANT_DIRECTORY_ITERATOR_TREC_GZ_RECURSIVE()
	-------------------------------------------------------------------------------------
*/
ANT_directory_iterator_trec_gz_recursive::~ANT_directory_iterator_trec_gz_recursive()
{
delete detrecer;
delete memory;
}

/*
	ANT_DIRECTORY_ITERATOR_TREC_GZ_RECURSIVE::NEW_PROVIDER()
	--------------------------------------------------------
*/
ANT_directory_iterator_file_buffered *ANT_directory_iterator_trec_gz_recursive::new_provider(char *filename)
{
delete detrecer;
delete memory;

memory = new ANT_memory(1024 * 1024);
file_stream = new ANT_instream_file(memory, filename);
#ifdef BUFFER_A
instream_buffer_a = new ANT_instream_buffer(memory, file_stream);
#else
instream_buffer_a = file_stream;
#endif

decompressor = new ANT_instream_deflate(memory, instream_buffer_a);

if (scrubbing_options != ANT_directory_iterator_scrub::NONE)
	scrubber = new ANT_instream_scrub(memory, decompressor, scrubbing_options);
else
	scrubber = decompressor;

#ifdef BUFFER_B
instream_buffer_b = new ANT_instream_buffer(memory, scrubber);
#else
instream_buffer_b = scrubber;
#endif

detrecer = new ANT_directory_iterator_file_buffered(instream_buffer_b, ANT_directory_iterator::READ_FILE);

return detrecer;
}

/*
	ANT_DIRECTORY_ITERATOR_TREC_GZ_RECURSIVE::NEXT()
	------------------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_trec_gz_recursive::next(ANT_directory_iterator_object *object)
{
ANT_directory_iterator_object *got;

START;

while (more_files != NULL)
	{
	END;
	if (first_time)
		got = detrecer->first(object);
	else
		got = detrecer->next(object);
	START;

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
		{
		END;
		return got;
		}
	}

END;
return NULL;
}
