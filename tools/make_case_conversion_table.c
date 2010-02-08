/*
	MAKE_CASE_CONVERSION_TABLE.C
	----------------------------
	This program reads the UnicodeData.txt file (see:unicode.org/Public/UNIDATA/UnicodeData.txt) and generates
	a language independant uppercase and lowercase conversion table. For details on the format of UnicodeData.txt
	see (http://www.unicode.org/Public/5.1.0/ucd/UCD.html#UnicodeData.txt).
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../source/disk.h"

enum { LOWER, UPPER };
/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
char *file, *pos;
char **lines, **current;
long long number_of_lines;
long field, mode, times;
long upper_character, lower_character, character, destination_character;

if (argc != 3)
	exit(printf("Usage:%s <-lower | -upper> <UnicodeData.txt>\n", argv[0]));

if (strcmp(argv[1], "-lower") == 0)
	mode = LOWER;
else if (strcmp(argv[1], "-upper") == 0)
	mode = UPPER;
else
	exit(printf("Usage:%s <-lower | -upper> <UnicodeData.txt>\n", argv[0]));

if ((file = ANT_disk::read_entire_file(argv[2])) == NULL)
	exit(printf("Cannot open input file:%s\n", argv[2]));

lines = ANT_disk::buffer_to_list(file, &number_of_lines);

if (mode == UPPER)
	printf("ANT_UNICODE_pair ANT_UNICODE_upper[] = {\n");
else
	printf("ANT_UNICODE_pair ANT_UNICODE_lower[] = {\n");

times = 0;
for (current = lines; *current != NULL; current++)
	{
	if (**current == '\0' || isspace(**current))
		continue;
	pos = *current;
	sscanf(pos, "%x", &character);
	for (field = 0; field < 12; field++)
		pos = strchr(pos + 1, ';');
	upper_character = lower_character = 0;
	sscanf(pos + 1, "%x", &upper_character);
	pos = strchr(pos + 1, ';');
	sscanf(pos + 1, "%x", &lower_character);

	if (mode == LOWER)
		destination_character = lower_character;
	else
		destination_character = upper_character;

	if (destination_character != 0)
		{
		if (times != 0)
			{
			printf(", ");
			if (times % 16 == 0)
				printf("\n");
			}
		printf("{%d, %d}", character, destination_character);
		times++;
		}


	}
printf("\n};\n");


return 0;
}