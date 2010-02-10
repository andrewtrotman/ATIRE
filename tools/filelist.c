/*
	FILELIST.C
	----------
*/
#include <stdio.h>
#include "../source/directory_recursive_iterator.h"
#include "../source/directory_recursive_iterator.h"

int main(int argc, char *argv[])
{
ANT_directory_recursive_iterator its(argc == 2 ? argv[1] : "*.*");
ANT_directory_iterator_object ob, *got;

for (got = its.first(&ob); got != NULL; got = its.next(&ob))
	puts(got->filename);
}