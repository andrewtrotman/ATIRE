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

file_stream = new ANT_instream_file(&memory, argv[1]);
decompressor = new ANT_instream_deflate(&memory, file_stream);
instream_buffer = new ANT_instream_buffer(&memory, decompressor);
source = new ANT_directory_iterator_tar(instream_buffer, ANT_directory_iterator::READ_FILE, ANT_directory_iterator_tar::NAME);

for (got = source->first(&raw_file); got != NULL; got = source->next(&raw_file))
	puts(raw_file.filename);

return 0;
}
