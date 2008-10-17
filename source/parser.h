/*
	PARSER.H
	--------
*/
#ifndef __PARSER_H__
#define __PARSER_H__

#include <ctype.h>
#include "string_pair.h"

class ANT_parser
{
private:
	char *document;
	char *current;
	ANT_string_pair current_token;

private:
	int isallowable(char val) { return isalpha(val) || val == '>' || val == '/'; }
	int isheadchar(char val) { return isalpha(val) || val == '<' || val == '\0'; }

public:
	ANT_parser();
	virtual ~ANT_parser();

	void set_document(char *document);
	ANT_string_pair *get_next_token(void);
} ;

#endif __PARSER_H__
