/*
	FILELIST.C
	----------
	test the file iterator code by listing all the files in a directory
*/
#include <stdio.h>
#include "../source/directory_iterator_recursive.h"

int main(int argc, char *argv[])
{
ANT_directory_iterator_recursive its((argc == 2 ? argv[1] : "*.*"));
ANT_directory_iterator_object ob, *got;

for (got = its.first(&ob); got != NULL; got = its.next(&ob))
	puts(got->filename);
}
