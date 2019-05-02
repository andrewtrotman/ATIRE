/*
	ATIRE_TAR.C
	-----------
*/
#include <stdio.h>

#include "memory.h"
#include "instream_file.h"
#include "instream_deflate.h"
#include "instream_buffer.h"
#include "directory_iterator_tar.h"
#include "directory_iterator_multiple.h"

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_memory memory;
ANT_instream *file_stream, *decompressor, *instream_buffer;
ANT_directory_iterator *source;
ANT_directory_iterator_object raw_file, *got;

ANT_directory_iterator_multiple *parallel_disk = new ANT_directory_iterator_multiple;

for (int file = 1; file < argc; file++)
	{
	file_stream = new ANT_instream_file(&memory, argv[file]);
	decompressor = new ANT_instream_deflate(&memory, file_stream);
	instream_buffer = new ANT_instream_buffer(&memory, decompressor);
	source = new ANT_directory_iterator_tar(instream_buffer, ANT_directory_iterator::READ_FILE, ANT_directory_iterator_tar::READ_FILE);
	
	parallel_disk->add_iterator(source);
	}

for (got = parallel_disk->first(&raw_file); got != NULL; got = parallel_disk->next(&raw_file))
	{
	printf("### %s\n", raw_file.filename);
	puts(raw_file.file);
	}

return 0;
}
