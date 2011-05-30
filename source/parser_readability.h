/*
	PARSER_READABILITY.H
	--------------------
*/
#ifndef PARSER_READABILITY_H_
#define PARSER_READABILITY_H_

#include "parser.h"

/*
	class PARSER_READABILITY
	------------------------
	Overrides the get_next_token method, to include end of sentence punctuation.
	Responsibility of the caller to covert to indexable tokens.
*/
class ANT_parser_readability : public ANT_parser
{
public:
	ANT_parser_readability() : ANT_parser() {};
	virtual ~ANT_parser_readability() {};

	virtual ANT_parser_token *get_next_token(void);

	static int issentenceend(unsigned char val) { return val == '.' || val == '!' || val == '?'; }
} ;

#endif  /* PARSER_READABILITY_H_ */
