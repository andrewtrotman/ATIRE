/*
	UNIVERSAL_PARSER.C
	------------------
	Created on: Jun 20, 2009
	Author: monfee
*/

#include "universal_parser.h"

/*
	ANT_UNIVERSAL_PARSER::ANT_UNIVERSAL_PARSER()
	--------------------------------------------
*/
ANT_universal_parser::ANT_universal_parser(ANT_encoding_factory::encoding what_encoding, bool by_char_or_word) : ANT_parser(), tokentype(by_char_or_word), current_lang(ANT_encoding::UNKNOWN)
{
enc = ANT_encoding_factory::gen_encoding_scheme(what_encoding);
}

/*
	ANT_UNIVERSAL_PARSER::ANT_UNIVERSAL_PARSER()
	--------------------------------------------
*/
ANT_universal_parser::ANT_universal_parser() :	ANT_parser(), tokentype(true), current_lang(ANT_encoding::UNKNOWN)
{
enc = ANT_encoding_factory::gen_encoding_scheme(ANT_encoding_factory::ASCII);
}

/*
	ANT_UNIVERSAL_PARSER::ANT_UNIVERSAL_PARSER()
	--------------------------------------------
*/
ANT_universal_parser::~ANT_universal_parser()
{
delete enc;
}

/*
	ANT_UNIVERSAL_PARSER::STORE_TOKEN()
	-----------------------------------
*/
void ANT_universal_parser::store_token(unsigned char *start)
{
ANT_encoding::language 		previous_lang = current_lang;
if (tokentype || current_lang == ANT_encoding::ENGLISH)
	{
/*
	TODO Chinese segmentation
*/
//		if (enc->lang() == ANT_encoding::CHINESE) {
//
//		}
	while (is_current_valid_char() && current_lang == previous_lang)
		{
		previous_lang = current_lang;
		enc->tolower(current);
		move2nextchar();
		}
	}

if (previous_lang == current_lang) // if the end of the token is not caused by the different language, we need to reset the variables
	initialise();

current_token.start = (char *)start;
current_token.string_length = current - start;
}

/*
	ANT_UNIVERSAL_PARSER::SET_DOCUMENT()
	--------------------------
*/
void ANT_universal_parser::set_document(unsigned char *document)
{
initialise();
this->document = current = document;
}

/*
	ANT_UNIVERSAL_PARSER::GET_NEXT_TOKEN()
	--------------------------------------
*/
ANT_string_pair *ANT_universal_parser::get_next_token(void)
{
unsigned char *start;

// avoid the cost in double checking for current utf8 valid char where there are many mixes of characters for different languages,
// e.g. linux[.], where [.] is a UTF-8 character in a language other than English
if (current_char_idc != ALPHACHAR)
	while ((current_char_idc = isheadchar(current)) == NOTHEADCHAR)
		current++;

if (current_char_idc == ALPHACHAR)				// alphabetic-like strings for all languages
	{
	current_lang = enc->lang();
	enc->tolower(current);
	start = current;
	move2nextchar();
	store_token(start);
	}
else if (current_char_idc == NUMBER)				// numbers
	{
	start = current++;
	while (ANT_isdigit(*current))
		current++;

	current_token.start = (char *)start;
	current_token.string_length = current - start;
	}
else if (current_char_idc == END)						// end of string
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


