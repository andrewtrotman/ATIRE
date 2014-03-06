/*
	PARSER_READABILITY.C
	--------------------
*/
#include "parser_readability.h"

/*
	ANT_PARSER_READABILITY::GET_NEXT_TOKEN()
	----------------------------------------
*/
ANT_parser_token *ANT_parser_readability::get_next_token(void)
{
unsigned char *start;
char *into, *from;

while (!ANT_isheadchar(*current) && !issentenceend(*current))
	current++;

current_token.string_length = 0;

if (issentenceend(*current))
	{
	start = current++;
	while (issentenceend(*current))
		current++;
	
	current_token.type = TT_PUNCTUATION;
	current_token.start = (char *)start;
	current_token.string_length = current - start;
	}
else if (ANT_isalpha(*current))
	{
	start = current++;
	while (ANT_isalpha(*current))
		current++;
	while (issentenceend(*current))
		current++;

	current_token.type = TT_WORD;
	current_token.start = (char *)start;
	current_token.string_length = current - start;
	for (into = current_token.normalized.start, from = (char *)start; into < current_token.normalized.start + current_token.string_length; into++)
		*into = ANT_tolower(*from++);
	current_token.normalized.string_length = current_token.string_length; 
	}
else if (ANT_isdigit(*current))
	{
	start = current++;
	while (ANT_isdigit(*current))
		current++;
	while (issentenceend(*current))
		current++;
	
	current_token.type = TT_NUMBER;
	current_token.start = (char *)start;
	current_token.string_length = current - start;
	current_token.string_length = current - start;
	for (into = current_token.normalized.start, from = (char *)start; into < current_token.normalized.start + current_token.string_length; into++)
		*into = *from++;
	current_token.normalized.string_length = current_token.string_length; 
	}
else if (*current == '\0')						// end of string
	return NULL;
else											// everything else (that starts with a '<')
	{
	start = current++; // keep the < so readability can know it's a tag
	if (ANT_isXMLnamestartchar(*current))
		{
		while (ANT_isXMLnamechar(*current))
			{
			*current = ANT_toupper(*current);
			current++;
			}

		current_token.type = TT_TAG_OPEN;
		current_token.start = (char *)start;
		current_token.string_length = current - start;

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
		}
	else
		{
		if (*current == '/')					// </tag>	(XML Close tag)
			{
			current_token.type = TT_TAG_CLOSE;
			current_token.start = (char *)++current;
			while (*current != '>')
				current++;
			current_token.string_length =  (char *)current - current_token.start;
			}
		else if (*current == '?')					// <? ... ?> (XML Processing Instructions)
			{
			current++; // current has to move to next character before we do the comparison again
			while (*current != '?' && *(current + 1) != '>')
				current++;
			}
		else if (*current == '!')				// <! ... > (
			{
			if (*(current + 1) == '-' && *(current + 2) == '-')		// <!-- /// --> (XML Comment)
				while (!((*current == '-') && (*(current + 1) == '-') && (*(current + 2) == '>')))
					current++;
			else								// nasty XML stuff like <![CDATA[<greeting>Hello, world!</greeting>]]>
				while (*current != '>')
					current++;
			}
		if (current_token.string_length == 0)
			return get_next_token();		// ditch and return the next character after where we are
		}
	}

return &current_token;
}
