/*
	PARSER_READABILITY.C
	--------------------
*/
#include "parser_readability.h"

/*
	ANT_PARSER_READABILITY::GET_NEXT_TOKEN()
	----------------------------------------
*/
ANT_string_pair *ANT_parser_readability::get_next_token(void)
{
unsigned char *start;

while (ANT_isspace(*current))
	current++;

if ((ANT_isalnum(*current) || ANT_ispunct(*current)) && *current != '<')
	{
	start = current++;
	while ((ANT_isalnum(*current) || ANT_ispunct(*current)) && *current != '<')
		current++;
	current_token.start = (char *)start;
	current_token.string_length = current - start;
	}
else if (*current == '\0')						// end of string
	return NULL;
else											// everything else (that starts with a '<')
	{
	while (*current != '>')
		{
		if (*current == '"')
			while (*current != '"')
				current++;
		else if (*current == '\'')
			while (*current != '\'')
				current++;
		current++;
		}
		current++; // skip over '>' itself
	return get_next_token(); // get the next token outside a tag
	}
return &current_token;
}
