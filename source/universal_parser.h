/*
	UNIVERSAL_PARSER.H
	------------------
	Created on: Jun 20, 2009
	Author: monfee
*/

#ifndef __UNIVERSAL_PARSER_H__
#define __UNIVERSAL_PARSER_H__

#include "parser.h"
#include "encoding_factory.h"

/*
	class ANT_UNIVERSAL_PARSER
	--------------------------
*/
class ANT_universal_parser : public ANT_parser
{
private:
	enum {NOTHEADCHAR = 0, ALPHACHAR = 1, NUMBER, LARROW, END};

private:
	ANT_encoding *enc;
	bool tokentype;  // true, parse the word as a single token; false, parse the single character as a token
	ANT_encoding::language lang;

protected:
	int isheadchar(unsigned char* val) { if (enc->is_valid_char(val)) return ALPHACHAR;
											else if (ANT_isdigit(*val)) return NUMBER;
											else if (*val == '<')  return LARROW;
											else if (*val == '\0')  return END;
											else return NOTHEADCHAR; }

public:
	ANT_universal_parser(ANT_encoding_factory::encoding what_encoding, bool tokentype/*by_char_or_word*/ = true);
	ANT_universal_parser();
	virtual ~ANT_universal_parser();

	ANT_string_pair *get_next_token(void);

private:
	inline void move2nextchar() { current += enc->howmanybytes(); }
	void store_token(unsigned char *start);
} ;

#endif __UNIVERSAL_PARSER_H__


