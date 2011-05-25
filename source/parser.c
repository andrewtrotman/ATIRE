/*
	PARSER.C
	--------
*/
#include <string.h>
#include "parser.h"
#include "plugin_manager.h"
#include "unicode.h"

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
this->should_segment = should_segment;

segmentation = NULL;
if ((this->should_segment & ONFLY_SEGMENTATION) == ONFLY_SEGMENTATION)
	if (!ANT_plugin_manager::instance().is_segmentation_plugin_available())
		this->should_segment -= ONFLY_SEGMENTATION;
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
long word_count = 0, pre_length_of_token = 0;
long character_bytes;
ANT_UNICODE_chartype chartype;
long is_chinese = 0;

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
	if (ANT_ispuncheadchar(*current))
		{
		chartype = CT_PUNCTUATION;
		break;
		}
	else if (*current & 0x80) //it is a unicode character
		{
		if (ischinese(current))
			{
			chartype = CT_LETTER;
			is_chinese = 1;
			break;
			}
		else if ((chartype = utf8_chartype(utf8_to_wide(current)))==CT_LETTER)
			{
			break;
			}
		/*
			We have a nasty case here (in ClueWeb09) that the last character of the file is a badly formed utf-8 character
			and so we have to check each character to see if we're at EOF yet
		*/
		for (character_bytes = utf8_bytes(current); character_bytes > 0; character_bytes--)
			if (*current++ == '\0')
				return NULL;
		}
	else
		current++;
	}

/*
	Now we look at the first character as it defines how parse the next token
*/

if (chartype == CT_LETTER && !is_chinese)
	{
	start = current;

	current = utf8_tolower(current);
	while (utf8_chartype(utf8_to_wide(current))==CT_LETTER)
		current = utf8_tolower(current);

	current_token.start = (char *)start;
	current_token.string_length = current - start;
	}
else if (chartype == CT_NUMBER)
	{
	start = current++;
	while (utf8_chartype(utf8_to_wide(current))==CT_NUMBER)
		current++;

	current_token.start = (char *)start;
	current_token.string_length = current - start;
	}
else if (*current == '\0')						// end of string
	return NULL;
else if (chartype == CT_PUNCTUATION && *current != '<')
	{
	start = current++;
	while (utf8_chartype(utf8_to_wide(current)) == CT_PUNCTUATION && *current != '<')		// this catches the case of punction before a tag "blah.</b>"
		current++;

	current_token.start = (char *)start;
	current_token.string_length = current - start;
	}
else if (is_chinese)
	{
	word_count = 1;
	start = current;
	current += utf8_bytes(current);		// move on to the next character

	if (should_segment)
		{
		while (ischinese(current))		// don't need to check for '\0' because that isn't a Chinese character
			{
			pre_length_of_token = utf8_bytes(current);
			current += pre_length_of_token;
			++word_count;
			if ((should_segment & BIGRAM_SEGMENTATION) == BIGRAM_SEGMENTATION && word_count >= 2)
				break;
			}

		if ((should_segment & ONFLY_SEGMENTATION) == ONFLY_SEGMENTATION)
			{
			segment(start, (long)(current - start));
			return get_next_token();
			}
		}

	current_token.start = (char *)start;
	current_token.string_length = current - start;

	// post-processing, for bigram indexing, needs move backward
	if ((should_segment & BIGRAM_SEGMENTATION) == BIGRAM_SEGMENTATION && ischinese(current))
		current -= pre_length_of_token;
	}
else											// everything else (that starts with a '<')
	{
	start = ++current;
	if (ANT_isXMLnamestartchar(*current))
		{
		while (ANT_isXMLnamechar(*current))
			{
			*current = ANT_toupper(*current);
			current++;
			}
		current_token.start = (char *)start;
		current_token.string_length = current - start;

		while (*current != '>' && *current != '\0')
			{
			if (*current == '"')
				do
					current++;
				while (*current != '"' && *current != '\0');
			else if (*current == '\'')
				do
					current++;
				while (*current != '\'' && *current != '\0');
			if (*current != '\0')
				current++;		// the else case as well as skipping over the quote
			}
		}
	else
		{
		if (*current != '\0')
			if (*current == '/')					// </tag>	(XML Close tag)
				{
				while (*current != '>' && *current != '\0')
					{
					*current = ANT_toupper(*current);
					current++;
					}
				/*
					New rules as of 11 Feb 2010, We return close tags as well as open tags
					because we need these for result focusing.
				*/
				current_token.start = (char *)start;
				current_token.string_length = current - start;
				return &current_token;
				}
			else if (*current == '?')					// <? ... ?> (XML Processing Instructions)
				{
				current++; // current has to move to next character before we do the comparison again
				if ((here = ::strstr(current, "?>")) == NULL)					// XML <? ?>
					if ((here = ::strchr(current, '>')) == NULL)				// HTML <? >
						here = current + strlen(current);					// End of document
				current = here;
				}
			else if (*current == '!')					// <! ... > (XML Comment)
				{
				if (*(current + 1) != '\0')
					if (*(current + 1) == '-' && *(current + 2) == '-')		// <!-- /// --> (XML Comment)
						{
						current += 2;
						if ((here = ::strstr(current, "-->")) == NULL)		// XML <!-- -->
							if ((here = ::strchr(current, '>')) == NULL)		// HTML <!-- >
								here = current + strlen(current);					// End of document
						current = here;
						}
					else								// nasty XML stuff like <![CDATA[<greeting>Hello, world!</greeting>]]>
						{
						if ((here = strchr(current, '>')) == NULL)
							here = current + strlen(current);
						current = here;
						}
				}
		return get_next_token();		// ditch and return the next character after where we are
		}
	}

return &current_token;
}
