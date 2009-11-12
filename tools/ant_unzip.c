/*
	ANT_UNZIP.C
	-----------
*/
#include <stdio.h>
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

unzipper = new ANT_directory_iterator_pkzip(argv[1]);

for (got = unzipper->first(&file, "", TRUE); got != NULL; got = unzipper->next(&file, TRUE))
	{
	puts("=====");
	puts(got->filename);
	puts(got->file);
	}


}