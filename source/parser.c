/*
	PARSER.C
	--------
*/
#include <ctype.h>
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
void ANT_parser::set_document(char *document)
{
this->document = current = document;
}

/*
	ANT_PARSER::GET_NEXT_TOKEN()
	----------------------------
*/
ANT_string_pair *ANT_parser::get_next_token(void)
{
char *start;

while (!isheadchar(*current))
	current++;
start = current++;
while (isallowable(*current))
	current++;

if (*start == '\0')
	return NULL;

current_token.start = start;
current_token.length = current - start;

return &current_token;
}
