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
ANT_parser::ANT_parser(long should_segment)
{
set_document(NULL);

segmentation = NULL;
if (should_segment && ANT_plugin_manager::instance().is_segmentation_plugin_available())
	this->should_segment = TRUE;
else
	this->should_segment = FALSE;
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
	ANT_PARSER::SEGMENT()
	---------------------
*/
void ANT_parser::segment(unsigned char *start, long length)
{
segmentation = (unsigned char *)ANT_plugin_manager::instance().do_segmentation(start, length);
}

/*
	ANT_PARSER::GET_NEXT_TOKEN()
	----------------------------
*/
ANT_string_pair *ANT_parser::get_next_token(void)
{
unsigned char *start, *here;

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
	start = here;
	while (!ANT_isspace(*here) && *here != '\0')
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

	if (current_token.string_length <= 0)
	    return get_next_token();
	return &current_token;
	}

/*
	We skip over non-indexable characters before the start of the first token
*/
for (;;)
	{
	if (isheadchar(*current))
		break;
	if (*current & 0x80) //it is a unicode character
		{
		if (ischinese(current))
			break;
		current += utf8_bytes(current);
		}
	//if(iseuropean(*current))
	//	break;
	else
		current++;
	}

/*
	Now we look at the first character as it defines how parse the next token
*/
if (ANT_isalpha(*current))	// alphabetic strings (in the ASCII CodePage)
	{
/*
	The call to iseuropean() below can never return true because ANT_isalpha() will alwasys be
	false in the case of a high-bit set character - all iseuropean() characters have the high
	bit set.
*/
/*
	if(iseuropean(*current)) 
		printf("und es ist ein %c",*current);
*/

	*current = ANT_tolower(*current); //first charater to lower and save it in start
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
			segment(start, (long)(current - start));
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
