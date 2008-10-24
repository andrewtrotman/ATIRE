/*
	INDEX.C
	-------
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "disk.h"
#include "parser.h"
#include "memory_index.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE 1
#endif

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_disk disk;
ANT_parser parser;
ANT_string_pair *token;
char *file;
long param;
ANT_memory_index *index;
long long doc;
long done_work;

if (argc < 2)
	exit(printf("Usage:%s <filespec> ...\n", argv[0]));
doc = 0;
index = new ANT_memory_index;
for (param = 1; param < argc; param++)
	for (file = disk.read_entire_file(disk.get_first_filename(argv[param])); file != NULL; file = disk.read_entire_file(disk.get_next_filename()))
		{
		done_work = FALSE;
		doc++;
		parser.set_document(file);
		while ((token = parser.get_next_token()) != NULL)
			{
			if (isalpha(*token->start))
				{
				index->add_term(token, doc);
				done_work = TRUE;
				}
			else if (token->length() == 5 && strncmp(token->start, "<DOC>", 5) == 0)
				if (done_work)
					doc++;
			}
		}
index->serialise();
delete index;
}

