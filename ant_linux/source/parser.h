/*
	PARSER.H
	--------
*/
#ifndef __PARSER_H__
#define __PARSER_H__

#include "string_pair.h"
#include "ctypes.h"

class ANT_parser
{
private:
	unsigned char *document;
	unsigned char *current;
	ANT_string_pair current_token;

private:
	int isallowable(unsigned char val) { return ANT_isalpha(val) || val == '>' || val == '/'; }
	int isheadchar(unsigned char val) { return ANT_isalpha(val) || val == '<' || val == '\0'; }

public:
	ANT_parser();
	virtual ~ANT_parser();

	void set_document(unsigned char *document);
	ANT_string_pair *get_next_token(void);
} ;

#endif /* __PARSER_H__ */
