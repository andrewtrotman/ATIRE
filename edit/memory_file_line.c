/*
	MEMORY_FILE_LINE.C
	------------------
*/
#include <stdio.h>
#include "../source/disk.h"
#include "memory_file_line.h"

/*
	ANT_MEMORY_FILE_LINE::ANT_MEMORY_FILE_LINE()
	--------------------------------------------
*/
ANT_memory_file_line::ANT_memory_file_line()
{
contents = NULL;
line = NULL;
current_line = 1;
current_line_pointer = NULL;
page_size = 10;		// lines of text per page
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
if (contents != NULL)
	{
	delete [] contents;
	delete [] line;
	}

contents = NULL;
line = NULL;
	
if ((contents = ANT_disk::read_entire_file(filename)) == NULL)
	return 0;

line = ANT_disk::buffer_to_list(contents, &lines_in_file);
file_start();

return lines_in_file;
}

/*
	ANT_MEMORY_FILE_LINE::LINE_UP()
	-------------------------------
*/
long long ANT_memory_file_line::line_up(void)
{
if (current_line_pointer == NULL)
	return current_line;

if (current_line_pointer == line)
	return current_line;

current_line--;
current_line_pointer--;

return current_line;
}

/*
	ANT_MEMORY_FILE_LINE::LINE_DOWN()
	---------------------------------
*/
long long ANT_memory_file_line::line_down(void)
{
if (current_line_pointer == NULL)
	return current_line;

if (*current_line_pointer != NULL)
	{
	current_line++;
	current_line_pointer++;
	}

return current_line;
}

/*
	ANT_MEMORY_FILE_LINE::PAGE_UP()
	-------------------------------
*/
long long ANT_memory_file_line::page_up(void)
{
long long which;

for (which = 0; which < page_size; which++)
	line_up();

return current_line;
}

/*
	ANT_MEMORY_FILE_LINE::PAGE_DOWN()
	---------------------------------
*/
long long ANT_memory_file_line::page_down(void)
{
long long which;

for (which = 0; which < page_size; which++)
	line_down();

return current_line;
}


/*
	ANT_MEMORY_FILE_LINE::FILE_START()
	----------------------------------
*/
long long ANT_memory_file_line::file_start(void)
{
current_line = 1;
current_line_pointer = line;

return current_line;
}

/*
	ANT_MEMORY_FILE_LINE::FILE_END()
	--------------------------------
*/
long long ANT_memory_file_line::file_end(void)
{
current_line = lines_in_file;
current_line_pointer = line + lines_in_file - 1;

return current_line;
}

/*
	ANT_MEMORY_FILE_LINE::GOTO_LINE()
	---------------------------------
*/
long long ANT_memory_file_line::goto_line(long long new_line)
{
current_line = new_line;
current_line_pointer = line + new_line - 1;


return current_line;
}