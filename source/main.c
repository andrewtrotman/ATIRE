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
long param;

for (param = 1; param < argc; param++)
	for (name = file.get_first_filename(argv[param]); name != NULL; name = file.get_next_filename())
		puts(name);
}