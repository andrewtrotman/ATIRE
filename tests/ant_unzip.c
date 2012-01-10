/*
	ANT_UNZIP.C
	-----------
	This program displays a list of the files stored in a ZIP archive.
*/
#include <stdio.h>
#include <stdlib.h>
#include "../source/directory_iterator_pkzip.h"

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_directory_iterator_pkzip *unzipper;
ANT_directory_iterator_object file, *got;

if (argc != 2)
	exit(printf("usage:%s <infile.zip>\n", argv[0]));

unzipper = new ANT_directory_iterator_pkzip(argv[1], ANT_directory_iterator::READ_FILE);

for (got = unzipper->first(&file); got != NULL; got = unzipper->next(&file))
	{
	puts("=====");
	puts(got->filename);
	puts(got->file);
	}
}
