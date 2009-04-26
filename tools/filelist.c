/*
	FILELIST.C
	----------
*/
#include <stdio.h>
#include "../source/directory_recursive_iterator.h"

int main(void)
{
ANT_directory_recursive_iterator its;
char *got;

for (got = its.first("*.*"); got != NULL; got = its.next())
	puts(got);
}