/*
	TEST_TAR.C
	----------
*/
#include <stdio.h>
#include <stdlib.h>
#include "../source/directory_iterator_tar.h"
#include "../source/instream_deflate.h"
#include "../source/instream_file.h"
#include "../source/memory.h"

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
if (argc != 2)
	exit(printf(""));

ANT_memory memory;
ANT_instream_file file(&memory, argv[1]);
ANT_instream_deflate source(&memory, &file);
ANT_directory_iterator_tar tarball(&source);
char *filename;

for (filename = tarball.first(""); filename != NULL; filename = tarball.next())
	puts(filename);
}