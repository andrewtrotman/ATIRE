/*
 * universal_parser.c
 *
 *  Created on: Jun 20, 2009
 *      Author: monfee
 */

#include "universal_parser.h"
#include "encoding_factory.h"

ANT_universal_parser::ANT_universal_parser(ANT_encoding::encoding what_encoding) :
		ANT_parse::ANT_parse()
{
	enc = encoding_factory::gen_encoding_scheme(what_encoding);
}


ANT_universal_parser::~ANT_universal_parser()
{
	if (enc != NULL)
		delete enc;
}

ANT_string_pair *ANT_universal_parser::get_next_token(void)
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

	current_token.start = (char *)start;
	current_token.string_length = current - start;
	}
else if (ANT_isdigit(*current))				// numbers
	{
	start = current++;
	while (ANT_isdigit(*current))
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
			{
			*current = ANT_toupper(*current);
			current++;
			}
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
			while (*current != '>')
				current++;
		else if (*current == '?')					// <? ... ?> (XML Processing Instructions)
			while (*current != '?' && *(current + 1) != '>')
				current++;
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
