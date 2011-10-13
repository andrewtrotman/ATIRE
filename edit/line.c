/*
	LINE.C
	------
*/
#include <new>
#include "line.h"

/*
	ANT_LINE::BUFFER_TO_LIST()
	--------------------------
	we see '\r'   seperated line on MacOS
	we see '\n'   seperated lines on Unix
	we see '\r\n' seperated lines on Windows

	but.. we cannot assume one file ending so we have to carefully check what we have at the end of each line
*/
ANT_line *ANT_line::buffer_to_list(char *buffer, long long *lines)
{
char *pos;
ANT_line *line_list, *current_line;
long long n_frequency, r_frequency, rn_frequency;

n_frequency = r_frequency = rn_frequency = 0;
for (pos = buffer; *pos != '\0'; pos++)
	if (*pos == '\n')
		n_frequency++;			// UNIX
	else if (*pos == '\r')
		{
		r_frequency++;			// MacOS
		if (*(pos + 1) == '\n')		
			{
			rn_frequency++;		// Windows
			pos++;
			}
		}

*lines = r_frequency + n_frequency;		// number of '\r' (or '\r\n') line ends plus the number of '\n' line ends without '\r's

current_line = line_list = new (std::nothrow) ANT_line [(size_t)(*lines + 2)]; 		// +1 in case the last line has no \n; +1 for a NULL at the end of the list

if (line_list == NULL)		// out of memory!
	return NULL;

current_line->text = pos = buffer;
current_line->attributes = NONE;
current_line++;
while (*pos != '\0')
	{
	if (*pos == '\n')
		{
		*pos++ = '\0';
		current_line->text = pos;
		current_line->attributes = NONE;
		current_line++;
		}
	else if (*pos == '\r')
		{
		if (*(pos + 1) == '\n')
			*pos++ = '\0';
		*pos++ = '\0';
		current_line->text = pos;
		current_line->attributes = NONE;
		current_line++;
		}		
	else
		pos++;
	}

current_line->text = NULL;
current_line->attributes = END_OF_FILE;
*lines = current_line - line_list - 1;		// the true number of lines we found (including possible missing '\n' at end of file

return line_list;
}
