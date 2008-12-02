/*
	LINK_EXTRACT.C
	--------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../source/disk.h"

char target[1024];
char anchor_text[1024 * 1024];

/*
	STRIP_SPACE_INLINE()
	--------------------
*/
char *strip_space_inline(char *source)
{
char *end, *start = source;

while (isspace(*start))
	start++;

if (start > source)
	memmove(source, start, strlen(start) + 1);		// copy the '\0'

end = source + strlen(source) - 1;
while ((end >= source) && (isspace(*end)))
	*end-- = '\0';

return source;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_disk disk;
char *file, *start, *end, *from, *ch;
char *target_start, *target_end, *target_dot;
long param, file_number;

if (argc < 2)
	exit(printf("Usage:%s <filespec> ...\n", argv[0]));

file_number = 1;
for (param = 1; param < argc; param++)
	{
	file = disk.read_entire_file(disk.get_first_filename(argv[param]));
	while (file != NULL)
		{
		from = file;
		while (from != NULL)
			{
			if ((start = strstr(from, "<collectionlink")) != NULL)
				if ((end = strstr(start, "</collectionlink>")) != NULL)
					{
					start = strstr(start, "xlink:href=");
					target_start = strchr(start, '"') + 1;
					target_dot = strchr(start, '.');
					target_end = strchr(target_start, '"');
					if (target_dot < target_end && target_dot != NULL)
						target_end = target_dot;
					strncpy(target, target_start, target_end - target_start);
					target[target_end - target_start] = '\0';

					start = strchr(start, '>') + 1;
					strncpy(anchor_text, start, end - start);
					anchor_text[end - start] = '\0';
					strip_space_inline(anchor_text);
					for (ch = anchor_text; *ch != '\0'; ch++)
						if (isspace(*ch))
							*ch = ' ';		// convert all spaces (tabs, cr, lf) into a space;
					printf("%s:%s\n", target, anchor_text);

					start = end;		// for the next time around the loop
					}
			from = start;
			}

		if (file_number % 1000 == 0)
			fprintf(stderr, "Files processed:%d\n", file_number);
		file_number++;

		delete [] file;
		file = disk.read_entire_file(disk.get_next_filename());
		}
	}
return 0;
}

