/*
	SOURCE_PARSER_C.H
	-----------------
*/
#ifndef SOURCE_PARSER_C_H_
#define SOURCE_PARSER_C_H_

#include "source_parser.h"

/*
	class ANT_SOURCE_PARSER_C
	-------------------------
*/
class ANT_source_parser_c : public ANT_source_parser
{
private:
	long long in_block_comment;		// is the parser currently in a block comment?

protected:
	static int reserved_word_cmp(const void *a, const void *b);

public:
	ANT_source_parser_c() : ANT_source_parser() { source = at = NULL; in_block_comment = false; }
	virtual ~ANT_source_parser_c() {}

	virtual long long rewind(void) { return in_block_comment = false; }

	virtual ANT_source_parser_token *next(void);
} ;

#endif /* SOURCE_PARSER_C_H_ */
