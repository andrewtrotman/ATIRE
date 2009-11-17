/*
	LINK_EXTRACT.C
	--------------
	Written (w) 2008 by Andrew Trotman, University of Otago
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../source/disk.h"
#include "../source/directory_recursive_iterator.h"
#include "link_parts.h"

#pragma warning(disable: 4706)		// assignment within conditional

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

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
char *file, *start, *end, *from, *ch;
char *target_start, *target_end, *target_dot;
char *slash;
long param, file_number, current_docid;
long lowercase_only, first_param;
ANT_directory_iterator_object file_object;
ANT_directory_iterator_object* file_object_tmp;

if (argc < 2)
	exit(printf("Usage:%s [-lowercase] <filespec> ...\n", argv[0]));

first_param = 1;
lowercase_only = FALSE;
if (*argv[1] == '-')
	{
	if (strcmp(argv[1], "-lowercase") == 0)
		{
		lowercase_only = TRUE;
		first_param = 2;
		}
	else
		exit(printf("Unknown parameter:%s\n", argv[1]));
	}

file_number = 1;
for (param = first_param; param < argc; param++)
	{
	ANT_directory_recursive_iterator disk(argv[param]);

	disk.first(&file_object);
	file = ANT_disk::read_entire_file(file_object.filename);
	while (file != NULL)
		{
		current_docid = get_doc_id(file);
		from = file;
		while (from != NULL)
			{
			if (((start = strstr(from, "<collectionlink")) != NULL) || ((start = strstr(from, "<link")) != NULL))
				if (((end = strstr(start, "</collectionlink>")) != NULL) || ((end = strstr(start, "</link>")) != NULL))
					{
					start = strstr(start, "xlink:href=");
					if (start != NULL && start < end)
						{
						target_start = strchr(start, '"') + 1;
						target_end = strchr(target_start, '"');

						/* skip / or \ */
						while ((slash = strpbrk(target_start, "\\/")) && slash < target_end)
							target_start = slash + 1;

						target_dot = strchr(target_start, '.');
						if (target_dot != NULL && target_dot < target_end)
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

						string_clean(anchor_text, lowercase_only);

						if (*target >= '0' && *target <= '9') // make sure this is a valid link
							printf("%d:%s:%s\n", current_docid, target, anchor_text);
						}

					if (start != NULL && start < end)
						start = end;		// for the next time around the loop
					}
			from = start;
			}

		if (file_number % 1000 == 0)
			fprintf(stderr, "Files processed:%d\n", file_number);
		file_number++;

		delete [] file;
		//file = disk.read_entire_file(disk.get_next_filename());
		file_object_tmp = disk.next(&file_object);
		if (!file_object_tmp)
		    break;
		file = ANT_disk::read_entire_file(file_object.filename);
		}
	}

fprintf(stderr, "%s Completed\n", argv[0]);
return 0;
}

