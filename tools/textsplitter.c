/*
	TEXTSPLITTER.C
	--------------
	Take a file, open it, split it into two and write it back out
	if the - flag is used the it just cleans the file
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/directory_iterator.h"
#include "../source/ctypes.h"
#include "../source/disk.h"

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_directory_iterator_object ob, *path;
ANT_directory_iterator its(argv[1], ANT_directory_iterator::READ_FILE);
char destination[1024];
char *filename, *ch;

if (argc != 2 && argc != 3)
	exit(printf("usage:%s <directory\\*.xml> [-]", argv[0]));

for (path = its.first(&ob); path != NULL; path = its.next(&ob))
	{
	/*
		Get the name of the file
	*/
	if ((filename = strrchr(path->filename, '\\')) == NULL)
		exit(printf("Cannot get filename from path:%s\n", filename));
	filename++;

	/*
		replace all non-alphanumerics with spaces
	*/
	for (ch = path->file; *ch != '\0'; ch++)
		if (!ANT_isalnum(*ch))
			*ch = ' ';

	if (argc == 3)
		{
		/*
			don't split the files, just clean them
		*/
		ch = path->file;
		sprintf(destination, "%s-a", filename);
		}
	else
		{
		/*
			Find the middle of the file without splitting a word
		*/
		for (ch = path->file + (path->length / 2); *ch != '\0'; ch++)
			if (ANT_isspace(*ch))
				break;
		*ch = '\0';

		/*
			Write the two parts
		*/
		sprintf(destination, "%s-a", filename);
//		puts(destination);
		ANT_disk::write_entire_file(destination, path->file, ch - path->file);
		ch++;
		sprintf(destination, "%s-b", filename);
		}
	ANT_disk::write_entire_file(destination, ch, strlen(ch));
//	puts(destination);
	}

return 0;
}
