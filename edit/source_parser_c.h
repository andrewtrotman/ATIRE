/*
	SOURCE_PARSER_C.H
	-----------------
*/
#ifndef SOURCE_PARSER_C_H_
#define SOURCE_PARSER_C_H_

#include "source_parser_token.h"

/*
	class ANT_SOURCE_PARSER_C
	-------------------------
*/
class ANT_source_parser_c
{
private:
	char *source;					// the line being parsed
	char *at;						// where in source we currently are
	ANT_source_parser_token token;	// this is where we build the token
	long long in_block_comment;		// is the parser currently in a block comment?

protected:
	static int reserved_word_cmp(const void *a, const void *b);

public:
	ANT_source_parser_c() { source = at = NULL; in_block_comment = false; }
	~ANT_source_parser_c() {}

	long long rewind(void) { in_block_comment = false; }
	char *set_text(char *text);

	ANT_source_parser_token *first(void);
	ANT_source_parser_token *next(void);
} ;

#endif /* SOURCE_PARSER_C_H_ */
