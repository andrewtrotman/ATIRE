/*
	UNIVERSAL_PARSER.H
	------------------
	Created on: Jun 20, 2009
	Author: monfee
*/

#ifndef __UNIVERSAL_PARSER_H__
#define __UNIVERSAL_PARSER_H__

#include "parser.h"
#include "encoding.h"
#include "encoding_factory.h"

/*
	class ANT_UNIVERSAL_PARSER
	--------------------------
*/
class ANT_universal_parser : public ANT_parser
{
private:
	enum {UNKNOWNCHAR = 0, ALPHACHAR = 1, NUMBER, LARROW, END, NOTHEADCHAR};

	ANT_encoding 				*enc;
	bool 						tokentype;  // true, parse the word as a single token; false, parse the single character as a token
	ANT_encoding::language 		current_lang;
	int 						current_char_idc; // current char indicator

private:
	void initialise();
	bool is_current_valid_char();
	void store_token(unsigned char *start);
	inline void move2nextchar() { current += enc->howmanybytes(); }

protected:
	int isheadchar(unsigned char *val);

public:
	ANT_universal_parser(ANT_encoding_factory::encoding what_encoding, bool tokentype/*by_char_or_word*/ = true);
	ANT_universal_parser();
	virtual ~ANT_universal_parser();

	void set_document(unsigned char *document);
	ANT_string_pair *get_next_token(void);
} ;

inline void ANT_universal_parser::initialise()
{
current_lang = ANT_encoding::UNKNOWN;
current_char_idc = UNKNOWNCHAR;
}

inline bool ANT_universal_parser::is_current_valid_char()
{
if (enc->is_valid_char(current))
	{
	current_char_idc = ALPHACHAR;
	current_lang = enc->lang();
	}
else
	current_char_idc = UNKNOWNCHAR;
return 	(current_char_idc == ALPHACHAR);
}

inline int ANT_universal_parser::isheadchar(unsigned char *val)
{
if (enc->is_valid_char(val)) return ALPHACHAR;
else if (ANT_isdigit(*val)) return NUMBER;
else if (*val == '<')  return LARROW;
else if (*val == '\0')  return END;
else return NOTHEADCHAR;
}

#endif __UNIVERSAL_PARSER_H__


