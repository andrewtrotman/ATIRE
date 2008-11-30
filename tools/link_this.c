/*
	LINK_THIS.C
	-----------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../source/disk.h"

/*
	CLEAN()
	-------
*/
char *clean(char *file)
{
char *ch;

ch = file;
while (*ch != '\0')
	{
	if (*ch == '<')			// remove the XML tags
		{
		while (*ch != '>')
			*ch++ = ' ';
		*ch++ = ' ';
		}
	else if (!isalnum(*ch))
		*ch++ = ' ';
	else
		ch++;
	}
return file;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
static char *seperators = " ";
ANT_disk disk;
char *file, *token;
char **term_list, **current;

if (argc != 2)
	exit(printf("Usage:%s <file>\n", argv[0]));

file = disk.read_entire_file(argv[1]);
clean(file);
puts(file);

current = term_list = new char *[strlen(file)];		// this is the worst case by far
for (token = strtok(file, seperators); token != NULL; token = strtok(NULL, seperators))
	*current++ = token;
*current = NULL;

for (current = term_list; *current != NULL; current++)
	puts(*current);

delete [] file;
}

