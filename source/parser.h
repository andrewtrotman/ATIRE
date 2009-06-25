/*
	PARSER.H
	--------
*/
#ifndef __PARSER_H__
#define __PARSER_H__

#include "string_pair.h"
#include "ctypes.h"

/*
	class ANT_PARSER
*/
class ANT_parser
{
protected:
	unsigned char *document;
	unsigned char *current;
	ANT_string_pair current_token;

protected:
	static int isheadchar(unsigned char val) { return ANT_isalnum(val) || val == '<' || val == '\0'; }

public:
	ANT_parser();
	virtual ~ANT_parser();

	static int isXMLnamestartchar(unsigned char val) { return ANT_isalpha(val) || val == ':' || val == '_'; }		// see http://www.w3.org/TR/REC-xml/#NT-NameStartChar
	static int isXMLnamechar(unsigned char val) { return isXMLnamestartchar(val) || ANT_isdigit(val) || val == '.' || val == '-'; } // see http://www.w3.org/TR/REC-xml/#NT-NameChar

	virtual void set_document(unsigned char *document);
	virtual ANT_string_pair *get_next_token(void);
} ;

#endif __PARSER_H__
