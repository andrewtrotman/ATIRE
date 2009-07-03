/*
	PARSER_READABILITY.H
	--------------------
*/
#ifndef __PARSER_READABILITY_H__
#define __PARSER_READABILITY_H__

#include "parser.h"

/*
	class PARSER_READABILITY
	------------------------
	Overrides the get_next_token method, to include punctuation and exclude tag names. 
	Responsibility of the caller to covert to indexable tokens.
*/
class ANT_parser_readability : public ANT_parser
{
public:
	ANT_parser_readability() : ANT_parser() {};
	virtual ~ANT_parser_readability() {};
	ANT_string_pair *get_next_token(void);
} ;

#endif __PARSER_READABILITY_H__
