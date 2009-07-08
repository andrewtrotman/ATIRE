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

while (!isheadchar(*current))
	current++;

if (ANT_isalpha(*current) || issentenceend(*current))				// alphabetic strings
	{
	start = current++;
	while (ANT_isalpha(*current) || issentenceend(*current))
		current++;

	current_token.start = (char *)start;
	current_token.string_length = current - start;
	}
else if (ANT_isdigit(*current) || issentenceend(*current))				// numbers
	{
	start = current++;
	while (ANT_isdigit(*current) || issentenceend(*current))
		current++;

	current_token.start = (char *)start;
	current_token.string_length = current - start;
	}
else if (*current == '\0')						// end of string
	return NULL;
else											// everything else (that starts with a '<')
	{
	start = ++current;
	if (isXMLnamestartchar(*current))
		{
		while (isXMLnamechar(*current))
			current++;

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
		return get_next_token(); // Don't care about tags
		}
	else
		{
		if (*current == '/')					// </tag>	(XML Close tag)
			while (*current != '>')
				current++;
		else if (*current == '?')					// <? ... ?> (XML Processing Instructions)
			{
			current++; // current has to move to next character before we do the comparison again
			while (*current != '?' && *(current + 1) != '>')
				current++;
			}
		else if (*current == '!')				// <! ... > (
			{
			if (*(current + 1) == '-' && *(current + 2) == '-')		// <!-- /// --> (XML Comment)
				while (*current != '-' && *(current + 1) != '-' && *(current + 2) != '>')
					current++;
			else								// nasty XML stuff like <![CDATA[<greeting>Hello, world!</greeting>]]>
				while (*current != '>')
					current++;
			}
		return get_next_token();		// ditch and return the next character after where we are
		}
	}

return &current_token;
}
