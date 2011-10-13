/*
	MEMORY_FILE_LINE_ITERATOR.C
	---------------------------
*/
#include <stdio.h>
#include "memory_file_line.h"
#include "memory_file_line_iterator.h"
#include "line.h"

/*
	ANT_MEMORY_FILE_LINE_ITERATOR::ANT_MEMORY_FILE_LINE_ITERATOR()
	--------------------------------------------------------------
*/
ANT_memory_file_line_iterator::ANT_memory_file_line_iterator(ANT_memory_file_line *source)
{
this->source = source;
top_of_screen = source->get_current_line();
}

/*
	ANT_MEMORY_FILE_LINE_ITERATOR::~ANT_MEMORY_FILE_LINE_ITERATOR()
	---------------------------------------------------------------
*/
ANT_memory_file_line_iterator::~ANT_memory_file_line_iterator()
{
}

/*
	ANT_MEMORY_FILE_LINE_ITERATOR::FIRST()
	--------------------------------------
*/
char *ANT_memory_file_line_iterator::first(void)
{
top_of_screen = source->get_current_line();

if (top_of_screen == NULL)
	return NULL;
else
	return top_of_screen->text;
}

/*
	ANT_MEMORY_FILE_LINE_ITERATOR::NEXT()
	-------------------------------------
*/
char *ANT_memory_file_line_iterator::next(void)
{
if (top_of_screen == NULL)
	return NULL;
else if (top_of_screen->attributes == ANT_line::END_OF_FILE)
	return NULL;
else
	{
	top_of_screen++;
	return top_of_screen->text;
	}
}