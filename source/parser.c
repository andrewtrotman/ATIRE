/*
	PARSER.C
	--------
*/
#include "parser.h"
#include "plugin_manager.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_PARSER::ANT_PARSER()
	------------------------
*/
ANT_parser::ANT_parser()
{
set_document(NULL);

#ifdef ONE_PARSER
	segmentation = NULL;
	should_segment = FALSE;
#endif
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

#ifdef ONE_PARSER
	/*
		ANT_PARSER::SEGMENT()
		---------------------
	*/
	void ANT_parser::segment(unsigned char *start, long length)
	{
	segmentation = (unsigned char *)ANT_plugin_manager::instance().do_segmentation(start, length);
	}
#endif

/*
	ANT_PARSER::GET_NEXT_TOKEN()
	----------------------------
*/
ANT_string_pair *ANT_parser::get_next_token(void)
{
unsigned char *start, *here;

#ifdef ONE_PARSER
	if (segmentation != NULL)
		{
		/*
			The segmenter has returned a space seperated list of tokens and we now have to work through them.
			My reading of Eric's Chinese Segmentation code is that each term is followed by a space and
			the final space is followed by a '\0'.  That is, the last term has a space after it.
		*/
		here = segmentation;
		while (ANT_isspace(*here))
			here++;
		start = here++;
		while (!ANT_isspace(*here))
			here++;

		/*
			Prepare for the next token (next call to get_next_token())
		*/
		if (*here == '\0')
			segmentation = NULL;		// no more tokens in this segmentation
		else
			segmentation = here;		// the next token continues on from here

		/*
			Build the token and return it
		*/
		current_token.start = (char *)start;
		current_token.string_length = here - start;

		return &current_token;
		}

#endif
#ifdef ONE_PARSER
	/*
		We skip over non-indexable characters before the start of the first token
	*/
	for (;;)
		{
		if (isheadchar(*current))
			break;
		if (*current & 0x80)
			{
			if (ischinese(current))
				break;
			current += utf8_bytes(current);
			}
		else
			current++;
		}

#else
	while (!isheadchar(*current))
		current++;
#endif
/*
	Now we look at the first character as it defines how parse the next token
*/
if (ANT_isalpha(*current))				// alphabetic strings (in the ASCII CodePage)
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
else if (ANT_isdigit(*current))				// numbers (in the ASCII CodePage)
	{
	start = current++;
	while (ANT_isdigit(*current))
		current++;

	current_token.start = (char *)start;
	current_token.string_length = current - start;
	}
else if (*current == '\0')						// end of string
	return NULL;
#ifdef ONE_PARSER
	else if (*current & 0x80)		// UTF-8 character
		{
		if (ischinese(current))		// Chinese CodePage
			{
			start = current;
			current += utf8_bytes(current);		// move on to the next character

			if (should_segment)
				{
				while (ischinese(current))		// don't need to check for '\0' because that isn't a Chinese character
					current += utf8_bytes(current);
				segment(start, current - start);
				return get_next_token();
				}
			else
				{
				current_token.start = (char *)start;
				current_token.string_length = current - start;
				}
			}
		/*
			There is no else clause because if the high bit is set we already know it must be Chinese
		*/
		}
#endif
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
		return get_next_token();		// ditch and return the next character after where we are
		}
	}

return &current_token;
}
