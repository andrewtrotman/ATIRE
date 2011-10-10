/*
	MEMORY_FILE_LINE.C
	------------------
*/
#include "memory_file_line.h"

/*
	ANT_MEMORY_FILE_LINE::ANT_MEMORY_FILE_LINE()
	--------------------------------------------
*/
ANT_memory_file_line::ANT_memory_file_line()
{
contents = NULL;
line = NULL;
}

/*
	ANT_MEMORY_FILE_LINE::~ANT_MEMORY_FILE_LINE()
	---------------------------------------------
*/
ANT_memory_file_line::~ANT_memory_file_line()
{
delete [] contents;
delete [] line;
}

/*
	ANT_MEMORY_FILE_LINE::READ_FILE()
	---------------------------------
*/
long long ANT_memory_file_line::read_file(char *filename)
{
long long lines_in_file;

if ((contents = ANT_disk::read_entire_file(filename)) == NULL)
	return 0;

line = ANT_disk::buiffer_to_list(contents, &lines_in_file);

return lines_in_file;
}
