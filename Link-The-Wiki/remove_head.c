/*
	REMOVE_HEAD.C
	-------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/disk.h"

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_disk disk;
char *text, *pos;
FILE *fp;

if (argc != 3)
	exit(printf("Usage:%s <infile><outfile>\n"));
text = disk.read_entire_file(argv[1]);
pos = strchr(text, '\n') + 1;
pos = strchr(pos, '\n') + 1;

fp = fopen(argv[2], "wb");
fprintf(fp, "%s", pos);
fclose(fp);

return 0;
}
