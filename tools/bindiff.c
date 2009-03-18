/*
	BINDIFF.C
	---------
*/

#include <stdio.h>
#include <stdlib.h>

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
FILE *a, *b;
int cha, chb;
long long pos;

if (argc != 3)
	exit(printf("Usage:%s <binfile1> <binfile2>\n", argv[0]));

a = fopen(argv[1], "rb");
b = fopen(argv[2], "rb");
pos = 0;

do
	{
	cha = fgetc(a);
	chb = fgetc(b);
	pos++;
	}
while (cha == chb && cha != EOF);

if (cha != EOF || chb != EOF)
	printf("diff at pos %lld\n", pos);
else
	puts("Files are the same");

fclose(a);
fclose(b);

return 0;
}
