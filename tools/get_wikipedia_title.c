/*
	GET_WIKIPEDIA_TITLE.C
	---------------------
*/
#include <stdio.h>
#include <string.h>
#include "..\source\ctypes.h"
#include "..\source\str.h"
#include "..\source\disk.h"

char buffer[1024 * 1024];

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_disk disk;
long param;
char *file, *filename;
char *start, *end, *ch;


for (param = 1; param < argc; param++)
	{
	file = disk.read_entire_file(filename = disk.get_first_filename(argv[param]));
	while (file != NULL)
		{
		start = strstr(file, "<name id=");
		start = strchr(start, '>') + 1;
		end = strstr(start, "</name>");
		strncpy(buffer, start, end - start);
		buffer[end - start] = '\0';

		for (ch = buffer; *ch != '\0'; ch++)
			if (*ch == '\n' || *ch == '\r')
				*ch = ' ';

		strip_space_inplace(buffer);
		printf("%s %s\n", filename, buffer);

		delete [] file;
		file = disk.read_entire_file(filename = disk.get_next_filename());
		}
	}

return 0;
}

