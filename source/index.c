/*
	INDEX.C
	-------
*/
#include <stdio.h>
#include <stdlib.h>
#include "disk.h"
#include "parser.h"

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

if (argc < 2)
	exit(printf("Usage:%s <filespec> ...\n", argv[0]));
for (param = 0; param < argc; param++)
	for (file = disk.read_entire_file(disk.get_first_filename(argv[param])); file != NULL; file = disk.read_entire_file(disk.get_next_filename()))
		{
		parser.set_document(file);
		while ((token = parser.get_next_token()) != NULL)
			puts(token->str());
		}
}

