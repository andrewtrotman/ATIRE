#include <stdio.h>
#include "disk.h"

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_disk file;
char *name;

for (name = file.get_first_filename(argv[1]); name != NULL; name = file.get_next_filename())
	puts(name);
}