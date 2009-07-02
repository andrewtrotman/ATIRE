/*
	READABILITY.H
	-------------
*/

#ifndef __READABILITY_H__
#define __READABILITY_H__

#include "parser.h"
#include "string_pair.h"

/* 
	class ANT_READABILITY
	---------------------
*/
class ANT_readability
{
public:
	ANT_readability() {};
	ANT_readability(ANT_parser *) { this->parser = parser; };
	virtual ~ANT_readability() {};

	virtual ANT_string_pair *get_next_token() {};
	virtual void set_document(unsigned char *) {};

protected:
	ANT_parser *parser;
} ;

#endif __READABILITY_H__
