/*
 * universal_parser.h
 *
 *  Created on: Jun 20, 2009
 *      Author: monfee
 */

#ifndef __UNIVERSAL_PARSER_H__
#define __UNIVERSAL_PARSER_H__

#include "parser.h"
#include "encoding_factory.h"

class ANT_universal_parser : public ANT_parser {
private:
	ANT_encoding* 	enc;
	bool			tokentype;  // true, parse the word as a single token; false, parse the single character as a token

protected:
	int isheadchar(unsigned char* val) { return enc->is_valid_char(val) || ANT_isdigit(*val) || *val == '<' || *val == '\0'; }

public:
	ANT_universal_parser(ANT_encoding_factory::encoding what_encoding, bool tokentype/*by_char_or_word*/ = true);
	ANT_universal_parser();
	virtual ~ANT_universal_parser();

	ANT_string_pair *get_next_token(void);

private:
	inline void move2nextchar() { current += enc->howmanybytes(); }
	void store_token(unsigned char *start);
};

#endif /* __UNIVERSAL_PARSER_H__ */
