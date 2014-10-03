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
set_document((char *)NULL);
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
//#define ASPT_UTF8_METHODS
ANT_parser_token *ANT_parser::get_next_token(void)
{
unsigned char *start, *here;
long word_count, pre_length_of_token;
unsigned long character;
unsigned char character_type;
size_t buffer_length;
char *buffer_pos;

#ifdef ASPT_UTF8_METHODS
long bytes;
char *buffer_end;
#endif

current_token.normalized.string_length = 0;

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
#ifdef ASPT_UTF8_METHODS
/*
	Start with the assumption that we're skipping over ASCII white space
*/
while (!ANT_isalnumpunczero(*current))
	current++;

/*
	If we've hit a non-white space then either its an ASCII non-white space,
	else we have to skip over UNICODE white space
*/
if (ANT_isascii(*current))
	{
	character_type = unicode_chartype_ASCII(*current);
	bytes = 1;
	}
else
	{
	/*
		Skipping over UNICODE whitespace
	*/
	for (;;)
		{
		character_type = (unsigned char)unicode_chartype_utf8(current, &character, &bytes);

		if (character == 0)
			return NULL;

		/*
			This is ordered on the most probable character we're going to see (so it isn't that dumb)
		*/
		if (character_type == CT_LETTER || character_type == CT_NUMBER || character_type == CT_PUNCTUATION || (character_type == CT_OTHER && (character == SPECIAL_TERM_CHAR || is_cjk_language(character))))
			break;

		current += bytes;
		}
	}
#else
for (;;)
	{
	character = utf8_to_wide(current);

	if (character == 0)
		return NULL;

	character_type = unicode_chartype_set(character);

	if (character_type == CT_LETTER || character_type == CT_NUMBER || character_type == CT_PUNCTUATION || (character_type == CT_OTHER && (character == SPECIAL_TERM_CHAR || is_cjk_language(character))))
		break;

	current += utf8_bytes(current);
	}
#endif

/*
	Now we look at the first character as it defines how parse the next token
*/
if (character_type == CT_LETTER)
	{
#ifdef ASPT_UTF8_METHODS
	start = current;

	//normalize the word into this buffer
	buffer_pos = current_token.normalized_buf;
	buffer_end = buffer_pos + sizeof(current_token.normalized_buf);

	if (bytes == 1)
		{
		current++;
		/*
			If the first character is ASCII then assume the remainder will be
		*/
		*buffer_pos++ = ANT_tolower(*start);

		while (ANT_isalpha(*current))
			{
			if (buffer_pos < buffer_end)
				*buffer_pos++ = ANT_tolower(*current);
			current++;
			}
		/*
			We might have terminated when we found a non-ASCII so keep processing from here on
			using UNICODE (i.e. the slow way)
		*/
		if ((*current & 0x80) != 0)
			{
			buffer_length = buffer_end - buffer_pos;
			while (unicode_chartype_utf8(current, &character, &bytes) == CT_LETTER)
				{
				ANT_UNICODE_normalize_lowercase_toutf8(&buffer_pos, &buffer_length, character);
				current += bytes;
				}
			}
		}
	else
		{
		current += bytes;
		/*
			The first character is not ASCII so assume more of the string won't be
			i.e. do it the slow way
		*/
		ANT_UNICODE_normalize_lowercase_toutf8(&buffer_pos, &buffer_length, character);
		while (unicode_chartype_utf8(current, &character, &bytes) == CT_LETTER)
			{
			ANT_UNICODE_normalize_lowercase_toutf8(&buffer_pos, &buffer_length, character);
			current += bytes;
			}
		}
	current_token.type = TT_WORD;
	current_token.normalized.string_length = buffer_pos - current_token.normalized_buf;
	current_token.start = (char *)start;
	current_token.string_length = current - start;
#else
	start = current;
	current += utf8_bytes(character);

	//normalize the word into this buffer
	buffer_pos = &current_token.normalized_buf[0];
	buffer_length = sizeof(current_token.normalized_buf);

	ANT_UNICODE_normalize_lowercase_toutf8(&buffer_pos, &buffer_length, character);

	while (character = utf8_to_wide(current), unicode_chartype(character) == CT_LETTER)
		{
		current += utf8_bytes(current);
		ANT_UNICODE_normalize_lowercase_toutf8(&buffer_pos, &buffer_length, character);
		}

	current_token.type = TT_WORD;
	current_token.normalized.string_length = buffer_pos - current_token.normalized_buf;
	current_token.start = (char *)start;
	current_token.string_length = current - start;
#endif
	}
else if (character_type == CT_NUMBER)
	{
	start = current;

#ifdef ASPT_UTF8_METHODS
	current += bytes;

	while (unicode_chartype_utf8(current, &character, &bytes) == CT_NUMBER)
		current += bytes;
#else
	current += utf8_bytes(current);

	while (character = utf8_to_wide(current), unicode_chartype(character)==CT_NUMBER)
		current += utf8_bytes(current);
#endif

	current_token.type = TT_NUMBER;
	current_token.start = (char *)start;
	current_token.string_length = current - start;
	}
else if (character_type == CT_PUNCTUATION && *current != '<')
	{
	start = current;

#ifdef ASPT_UTF8_METHODS
	current += bytes;
	while (unicode_chartype_utf8(current, &character, &bytes) == CT_PUNCTUATION && character != '<')		// this catches the case of punction before a tag "blah.</b>"
		current += utf8_bytes(current);
#else
	current += utf8_bytes(current);
	while (character = utf8_to_wide(current), unicode_chartype(character) == CT_PUNCTUATION && character != '<')		// this catches the case of punction before a tag "blah.</b>"
		current += utf8_bytes(current);
#endif

	current_token.type = TT_PUNCTUATION;
	current_token.start = (char *)start;
	current_token.string_length = current - start;
	}
else if (character_type == CT_ZERO)
	return NULL;			// at end of document
else if (character_type == CT_OTHER && character == SPECIAL_TERM_CHAR)
	{
	/* Special term which begins with the 0x80 marker and ends after the first non-alnum or non -: */
	start = current;

	//The term marker is multi-byte, but the following characters will be single byte
#ifdef ASPT_UTF8_METHODS
	current += bytes;
#else
	current += utf8_bytes(current);
#endif

	while (ANT_isalnum(*current) || *current == '-' || *current == ':')
		current += sizeof(*current);

	current_token.type = TT_WORD;
	current_token.start = (char *)start;
	current_token.string_length = current - start;
	}
else if (character_type == CT_OTHER && is_cjk_language(character))
	{
	pre_length_of_token = 0;
	word_count = 1;
	start = current;

#ifdef ASPT_UTF8_METHODS
	current += bytes;
#else
	current += utf8_bytes(current);
#endif

	if (should_segment)
		{
		while (is_cjk_language(current))		// don't need to check for '\0' because that isn't a Chinese character
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

	current_token.type = TT_WORD;
	current_token.start = (char *)start;
	current_token.string_length = current - start;

	// post-processing, for bigram indexing, needs move backward
	if ((should_segment & BIGRAM_SEGMENTATION) == BIGRAM_SEGMENTATION && is_cjk_language(current))
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
		current_token.type = TT_TAG_OPEN;
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
				start = ++current;
				while (*current != '>' && *current != '\0')
					{
					*current = ANT_toupper(*current);
					current++;
					}
				/*
					New rules as of 11 Feb 2010, We return close tags as well as open tags
					because we need these for result focusing.
				*/
				current_token.type = TT_TAG_CLOSE;
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

/*
	ANT_PARSER::GET_SEGMENT_INFO()
	------------------------------
*/
long ANT_parser::get_segment_info()
{
return this->should_segment;
}

/*
	ANT_PARSER::SET_SEGMENT_INFO()
	----------------------------
*/
void ANT_parser::set_segment_info(long should_segment)
{
this->should_segment = should_segment;
}
