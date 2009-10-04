/*
	MAIN.C
	------
*/
#include <stdio.h>
#include <stdlib.h>
#include "instream_file.h"
#include "instream_deflate.h"
#include "..\source\memory.h"

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
if (argc != 2)
	exit(printf("Usage:%s <infile>\n", argv[0]));

ANT_memory memory;
ANT_instream_file file(&memory, argv[1]);
ANT_instream_deflate deflate(&memory, &file);
long long got;
unsigned char buffer[16];

do
	{
	got = deflate.read(buffer, sizeof(buffer));
	if (got > 0)
		{
		fwrite(buffer, got, 1, stdout);
		fflush(stdout);
		}
	}
while (got > 0);
}

