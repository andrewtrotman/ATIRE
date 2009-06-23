/*
 * universal_parser.c
 *
 *  Created on: Jun 20, 2009
 *      Author: monfee
 */

#include "universal_parser.h"

ANT_universal_parser::ANT_universal_parser(ANT_encoding_factory::encoding what_encoding
											, bool by_char_or_word) :
		ANT_parser::ANT_parser(), tokentype(by_char_or_word)
{
	enc = ANT_encoding_factory::gen_encoding_scheme(what_encoding);
}

ANT_universal_parser::ANT_universal_parser() :
		ANT_parser::ANT_parser(), tokentype(true)
{
	enc = ANT_encoding_factory::gen_encoding_scheme(ANT_encoding_factory::ASCII);
}

ANT_universal_parser::~ANT_universal_parser()
{
	if (enc != NULL)
		delete enc;
}

void ANT_universal_parser::store_token(unsigned char *start)
{
	if (tokentype || enc->is_english()) {

		/** TODO Chinese segmentation
		 *
		 */
//		if (enc->lang() == ANT_encoding::CHINESE) {
//
//		}
		while (enc->is_valid_char(current))
			{
			enc->tolower(current);
			move2nextchar();
			}
	}

	current_token.start = (char *)start;
	current_token.string_length = current - start;
}

ANT_string_pair *ANT_universal_parser::get_next_token(void)
{
unsigned char *start;

while (!isheadchar(current))
	current++;

if (enc->is_valid_char(current))				// alphabetic strings for all languages
	{
	enc->tolower(current);
	start = current;
	move2nextchar();
	store_token(start);
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
			enc->toupper(current);
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


