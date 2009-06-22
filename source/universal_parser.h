/*
 * universal_parser.h
 *
 *  Created on: Jun 20, 2009
 *      Author: monfee
 */

#ifndef __UNIVERSAL_PARSER_H__
#define __UNIVERSAL_PARSER_H__

#include "char.h"
#include "encoding.h"

class ANT_universal_parser {
private:
	ANT_char *document;
	ANT_char *current;
	ANT_string_pair current_token;
	ANT_encoding* enc;

protected:
	static int isheadchar(unsigned char val) { return ANT_isalnum(val) || val == '<' || val == '\0'; }

public:
	ANT_universal_parser();
	virtual ~ANT_universal_parser();

	static int isXMLnamestartchar(unsigned char val) { return ANT_isalpha(val) || val == ':' || val == '_'; }		// see http://www.w3.org/TR/REC-xml/#NT-NameStartChar
	static int isXMLnamechar(unsigned char val) { return isXMLnamestartchar(val) || ANT_isdigit(val) || val == '.' || val == '-'; } // see http://www.w3.org/TR/REC-xml/#NT-NameChar

	void set_document(unsigned char *document);
	ANT_string_pair *get_next_token(void);
};

#endif /* __UNIVERSAL_PARSER_H__ */
