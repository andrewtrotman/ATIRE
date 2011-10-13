/*
	SOURCE_PARSER.C
	---------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "source_parser.h"

/*
	ANT_SOURCE_PARSER::SET_TEXT()
	-----------------------------
*/
char *ANT_source_parser::set_text(char *text)
{
return at = source = text;
}

/*
	ANT_SOURCE_PARSER::FIRST()
	--------------------------
*/
ANT_source_parser_token *ANT_source_parser::first(void)
{
return next();
}

/*
	ANT_SOURCE_PARSER::NEXT()
	-------------------------
*/
ANT_source_parser_token *ANT_source_parser::next(void)
{

if (at == NULL)
	return NULL;

if (*at == '\0')
	return NULL;

at++;

token.string_start = at;
token.string_length = 1;
token.string_attributes = ANT_source_parser_token::NONE;
token.string_type = ANT_source_parser_token::UNKNOWN;

return &token;
}
