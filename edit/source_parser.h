/*
	SOURCE_PARSER.H
	---------------
*/
#ifndef SOURCE_PARSER_H_
#define SOURCE_PARSER_H_

#include "source_parser_token.h"

/*
	class ANT_SOURCE_PARSER
	-----------------------
*/
class ANT_source_parser
{
protected:
	char *source;					// the line being parsed
	char *at;						// where in source we currently are
	ANT_source_parser_token token;	// this is where we build the token

public:
	ANT_source_parser() { source = at = NULL; }
	virtual ~ANT_source_parser() {}

	virtual long long rewind(void) { return 0; }
	virtual char *set_text(char *text);

	virtual ANT_source_parser_token *first(void);
	virtual ANT_source_parser_token *next(void);
} ;



#endif /* SOURCE_PARSER_H_ */
