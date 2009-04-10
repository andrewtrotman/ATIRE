/*
	PARSER.C
	--------
*/
#include "parser.h"

/*
	ANT_PARSER::ANT_PARSER()
	------------------------
*/
ANT_parser::ANT_parser()
{
set_document(NULL);
}

/*
	ANT_PARSER::~ANT_PARSER()
	-------------------------
*/
ANT_parser::~ANT_parser()
{
}

/*
	ANT_PARSER::SET_DOCUMENT()
	--------------------------
*/
void ANT_parser::set_document(unsigned char *document)
{
this->document = current = document;
}

/*
	ANT_PARSER::GET_NEXT_TOKEN()
	----------------------------
*/
ANT_string_pair *ANT_parser::get_next_token(void)
{
unsigned char *start;

while (!isheadchar(*current))
	current++;

if (ANT_isalpha(*current))				// alphabetic strings
	{
	*current = ANT_tolower(*current);
	start = current++;
	while (ANT_isalpha(*current))
		{
		*current = ANT_tolower(*current);
		current++;
		}
	}
else if (ANT_isdigit(*current))				// numbers
	{
	start = current++;
	while (ANT_isdigit(*current))
		current++;
	}
else if (*current == '\0')						// end of string
	return NULL;
else											// everything else (that starts with a '<'
	{
	start = current++;
	while (isallowable(*current))
		current++;
	}

current_token.start = (char *)start;
current_token.string_length = current - start;

return &current_token;
}
