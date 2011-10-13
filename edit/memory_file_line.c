/*
	MEMORY_FILE_LINE.C
	------------------
*/
#include <stdio.h>
#include <string.h>
#include <new>
#include "../source/disk.h"
#include "memory_file_line.h"
#include "point.h"
#include "memory_file_line_iterator.h"
#include "edit_renderer.h"
#include "source_parser_c.h"
#include "token_colours.h"
#include "line.h"

/*
	ANT_MEMORY_FILE_LINE::ANT_MEMORY_FILE_LINE()
	--------------------------------------------
*/
ANT_memory_file_line::ANT_memory_file_line(ANT_edit_renderer *renderer)
{
contents = NULL;
line = NULL;
current_line = 1;
lines_in_file = 0;
current_line_pointer = NULL;
page_size = 10;		// lines of text per page
this->renderer = renderer;
window_width = window_height = 0;
}

/*
	ANT_MEMORY_FILE_LINE::~ANT_MEMORY_FILE_LINE()
	---------------------------------------------
*/
ANT_memory_file_line::~ANT_memory_file_line()
{
delete [] contents;
delete [] line;
delete renderer;
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

line = ANT_line::buffer_to_list(contents, &lines_in_file);
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

if (current_line_pointer->text != NULL)
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

/*
	ANT_MEMORY_FILE_LINE::RENDER()
	------------------------------
*/
long long ANT_memory_file_line::render(void)
{
ANT_rgb *colour;
ANT_token_colours colour_set;
ANT_point where, size;
ANT_memory_file_line_iterator iterator(this);
ANT_source_parser *parser;
ANT_source_parser_token *token;
char *current_line;
long long max_y;
ANT_rgb *rgb_colour;

if ((current_line = iterator.first()) == NULL)
	return 0;

parser = new (std::nothrow) ANT_source_parser_c();
where.x = where.y = size.x = size.y = 0;
while (where.y < window_height)
	{
	max_y = where.x = 0;
	if (current_line == NULL || *current_line == '\0')		// blank lines
		{
		renderer->render_text_segment(&where, colour_set.colour(ANT_source_parser_token::KNOWN_ERROR), " ", 1, &size);
		max_y = size.y;
		}
	else
		{
		parser->set_text(current_line);
		for (token = parser->first(); token != NULL; token = parser->next())
			{
			if (token->type() == ANT_source_parser_token::TAB)
				renderer->render_text_segment(&where, colour_set.colour(token->type()), "   ", 3, &size);
			else
				{
				if (token->attributes() & ANT_source_parser_token::ATTRIBUTE_BLOCK_COMMENT)
					rgb_colour = colour_set.colour(ANT_source_parser_token::BLOCK_COMMENT);
				else if (token->type() == ANT_source_parser_token::CLOSE_BLOCK_COMMENT)
					rgb_colour = colour_set.colour(ANT_source_parser_token::KNOWN_ERROR);
				else
					rgb_colour = colour_set.colour(token->type());

				if (token->attributes() & ANT_source_parser_token::ATTRIBUTE_UNICODE)
					renderer->render_utf8_segment(&where, rgb_colour, token->string(), token->length(), &size);
				else
					renderer->render_text_segment(&where, rgb_colour, token->string(), token->length(), &size);
				}

			where.x += size.x;
			if (size.y > max_y)
				max_y += size.y;
			}
		}

	if ((current_line = iterator.next()) == NULL)
		break;

	where.y += max_y;
	}

delete parser;
return where.y;
}