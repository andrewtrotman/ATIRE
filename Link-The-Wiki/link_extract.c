/*
	LINK_EXTRACT.C
	--------------
	Written (w) 2008 by Andrew Trotman, University of Otago
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _MSC_VER
	#define strcasecmp _stricmp
#endif

#include "../source/disk.h"
#include "../source/directory_iterator_recursive.h"
#include "link_parts.h"

#pragma warning(disable: 4706)		// assignment within conditional

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

#ifdef INEX_ARCHIVE
	#define COLLECTION_LINK_TAG_NAME "collectionlink"
#else
	#define COLLECTION_LINK_TAG_NAME "link"
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
char *file, *start, *end, *from, *ch, *pos, *anchor_start, *buffer_start, *paragraph_start, *p_letter, *lt_letter/*<p>*/;
char *target_start, *target_end, *target_dot;
char *slash;
long param, file_number, current_docid, source_docid, target_docid;
long lowercase_only, first_param, crosslink, append_source_title;
long to_continue, language_link;
ANT_directory_iterator_object file_object;
ANT_directory_iterator_object* file_object_tmp;

if (argc < 2)
	exit(printf("Usage:%s [-crosslink] [-append-source-title] [-lowercase] <filespec> ...\n", argv[0]));

first_param = 1;
lowercase_only = FALSE;
language_link = FALSE;
crosslink = FALSE;
append_source_title = FALSE;
char *command;
const char *default_namespace = "en";
const char *crosslink_namespace = default_namespace;
char current_namespace[1024];
char buffer[1024 * 1024];
char doctitle[1024 * 1024];
char link_title[1024 * 1024];

for (param = 1; param < argc; param++)
	{
	if (*argv[param] == '-')
		{
		command = argv[param] + 1;
		if (strncmp(command, "crosslink", 9) == 0)
			{
			crosslink = TRUE;
			if (strlen(command) > 10 && command[9] == ':')
				crosslink_namespace = command + 10;
			++first_param;
			}
		else if (strcmp(command, "append-source-title") == 0)
			{
			append_source_title = TRUE;
			++first_param; // = 2;
			}
		else if (strcmp(command, "lowercase") == 0)
			{
			lowercase_only = TRUE;
			++first_param; // = 2;
			}
		else
			exit(printf("Unknown parameter:%s\n", argv[1]));
		}
	}

file_number = 1;
for (param = first_param; param < argc; param++)
	{
	ANT_directory_iterator_recursive disk(argv[param]);

	disk.first(&file_object);
	file = ANT_disk::read_entire_file(file_object.filename);
	while (file != NULL)
		{
		current_docid = get_doc_id(file);
		get_doc_name(file, doctitle);
		from = file;
		language_link = FALSE;

		while (from != NULL)
			{
			if (((start = strstr(from, "<"COLLECTION_LINK_TAG_NAME)) != NULL))
				if (((end = strstr(start, "</"COLLECTION_LINK_TAG_NAME">")) != NULL))
					{
					anchor_start = strchr(start + strlen(COLLECTION_LINK_TAG_NAME), '>');
					++anchor_start;

					memcpy(buffer, start, end - start);
					buffer[end - start] = '\0';
					*current_namespace = '\0';
					target_docid = -1;
					to_continue = FALSE;

					// crosslink link has to have this attribute
					if ((buffer_start = strstr(buffer, "xlink:label=\"")) != NULL)
						{
						// check if it is a language link
						if (!language_link)
							{
							(paragraph_start = start)--;
//							if (isspace(*paragraph_start))
							while (isspace(*paragraph_start))
								--paragraph_start;

							if (*paragraph_start == '>')
								{
								p_letter = --paragraph_start;
								lt_letter = --paragraph_start;
								if (*p_letter == 'p' && *lt_letter == '<')
									language_link = TRUE;
								}
							}

						buffer_start += strlen("xlink:label=\"");
						pos = strchr(buffer_start, '"');
						if (pos == NULL || (pos - buffer) >= 1024)
							fprintf(stderr, "This file contain error link information: %d\n", current_docid);
						else
							{
							strncpy(current_namespace, buffer_start, pos - buffer_start);
							current_namespace[pos - buffer_start] = '\0';
							if (strcasecmp(current_namespace, crosslink_namespace) == 0)
								{
								if ((buffer_start = strstr(buffer, "xlink:title=\"")) != NULL)
									{
									buffer_start += strlen("xlink:title=\"");
									pos = strchr(buffer_start, '"');
									memcpy(link_title, buffer_start, pos - buffer_start);
									link_title[pos - buffer_start] = '\0';
									to_continue = TRUE;
									}
								}
							}
						}
					else
						if (!crosslink)
							to_continue = TRUE;

					if (!to_continue) {
						from = end;
						continue;
					}

					buffer_start = strstr(buffer, "xlink:href=");
					if (buffer_start != NULL/* && buffer_start < end*/)
						{
						target_start = strchr(buffer_start, '"') + 1;
						target_end = strchr(target_start, '"');

						/* skip / or \ */
						while ((slash = strpbrk(target_start, "\\/")) && slash < target_end)
							target_start = slash + 1;

						target_dot = strchr(target_start, '.');
						if (target_dot != NULL && target_dot < target_end)
							target_end = target_dot;
						strncpy(target, target_start, target_end - target_start);
						target[target_end - target_start] = '\0';

						if (*target >= '0' && *target <= '9') // make sure this is a valid link
							target_docid = atol(target);
						}

//					start = strchr(start, '>') + 1;
					strncpy(anchor_text, anchor_start, end - anchor_start);
					anchor_text[end - anchor_start] = '\0';
					strip_space_inline(anchor_text);
					for (ch = anchor_text; *ch != '\0'; ch++)
						if (isspace(*ch))
							*ch = ' ';		// convert all spaces (tabs, cr, lf) into a space;

					string_clean(anchor_text, lowercase_only);

					if (crosslink)
						{
						/*
						 * Actually, depend on the language of corpus which we want to extract the crosslink from, the direction of link could be different.
						 * I would like to use Chinese document to extract English link for now which mean we don't have the source document id.
						 */
							if (strlen(anchor_text) == 0) {// the language link doesn't have an anchor
//								printf("0:%d:%s\n", current_docid, link_title);
								strcpy(anchor_text, link_title);

								if (language_link)
									{
									if (append_source_title) {
										strcat(anchor_text, "|");
										strcat(anchor_text, doctitle);
									}
									if (target_docid > 0)
										source_docid = target_docid;
									else
										source_docid = 0;
									target_docid = current_docid;
									}
								else
									{
									source_docid = current_docid;
									if (target_docid < 0)
										target_docid = 0;
									}
							}
							else
								*anchor_text = '\0';
						}
					else
						source_docid = current_docid;

					if (target_docid > -1 && strlen(anchor_text) > 0)
						printf("%d:%d:%s\n", source_docid, target_docid, anchor_text);

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

