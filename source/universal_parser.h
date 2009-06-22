/*
 * universal_parser.h
 *
 *  Created on: Jun 20, 2009
 *      Author: monfee
 */

#ifndef __UNIVERSAL_PARSER_H__
#define __UNIVERSAL_PARSER_H__

#include "parser.h"
#include "encoding.h"

class ANT_universal_parser : public ANT_parser {
private:
	ANT_encoding* enc;

protected:
	static int isheadchar(unsigned char* val) { return enc->is_valid_char(val) || ANT_isdigit(*val) || *val == '<' || *val == '\0'; }

public:
	ANT_universal_parser(ANT_encoding::encoding what_encoding);
	virtual ~ANT_universal_parser();

	ANT_string_pair *get_next_token(void);
};

#endif /* __UNIVERSAL_PARSER_H__ */
