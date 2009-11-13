/*
	FILELIST.C
	----------
*/
#include <stdio.h>
#include "../source/directory_recursive_iterator.h"
#include "../source/directory_recursive_iterator.h"

int main(void)
{
ANT_directory_recursive_iterator its("*.*");
ANT_directory_iterator_object ob, *got;

for (got = its.first(&ob, "*.*"); got != NULL; got = its.next(&ob))
	puts(got->filename);
}