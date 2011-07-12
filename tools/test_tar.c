/*
	TEST_TAR.C
	----------
*/
#include <stdio.h>
#include <stdlib.h>
#include "../source/directory_iterator_warc.h"
#include "../source/directory_iterator_tar.h"
#include "../source/instream_deflate.h"
#include "../source/instream_bz2.h"
#include "../source/instream_file.h"
#include "../source/memory.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
if (argc != 2)
	exit(printf("Nope"));

ANT_memory memory;
ANT_directory_iterator_object file_object, *current;
ANT_instream_file file(&memory, argv[1]);
ANT_instream_bz2 source(&memory, &file);
ANT_directory_iterator_tar tarball(&source, ANT_directory_iterator::READ_FILE);
//ANT_instream_deflate source(&memory, &file);
//ANT_directory_iterator_warc tarball(&source, ANT_directory_iterator::READ_FILE);

for (current = tarball.first(&file_object); current != NULL; current = tarball.next(&file_object))
	{
	puts(current->filename);
	delete [] current->file;
	delete [] current->filename;
	}
}
