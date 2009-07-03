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
	virtual ~ANT_readability() {};
	
	virtual ANT_string_pair *get_next_token() = 0;
	virtual void set_document(unsigned char *) = 0;
	virtual long score() = 0;
	
	void set_parser(ANT_parser *parser) { this->parser = parser; }

protected:
	ANT_parser *parser;
} ;

#endif __READABILITY_H__
