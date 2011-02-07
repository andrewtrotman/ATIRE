/*
	LINK_ANALYSE_INEX_WIKIPEDIA.C
	-----------------------------
	(at present) check that the link graph fits intop long integers and does not overflow
	since it does, this program is now redundant.
*/
#include <stdio.h>
#include <stdlib.h>
#include "../source/disk.h"

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
char *end, *data;
long long *current, file_length, param;
long as_long;

for (param = 1; param < argc; param++)
	{
	data = ANT_disk::read_entire_file(argv[param], &file_length);
	end = data + file_length;

	for (current = (long long *)data; current < (long long *)end; current++);
		{
		as_long = (long)*current;
		if (*current != (long long)as_long)
			exit(printf("BROKEN\n"));
		}
	delete [] data;
	}
}
