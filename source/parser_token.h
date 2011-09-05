/*
	PARSER_TOKEN.H
	-------------
*/
#ifndef PARSER_TOKEN_H_
#define PARSER_TOKEN_H_

#include <stdio.h>
#include "string_pair.h"
#include "pragma.h"
#include "memory.h"
#include "str.h"
#include "btree_iterator.h"

enum ANT_parser_token_type {TT_INVALID, TT_WORD, TT_NUMBER, TT_TAG_OPEN, TT_TAG_CLOSE, TT_PUNCTUATION};

/*
	class ANT_PARSER_TOKEN
	---------------------
*/
class ANT_parser_token : public ANT_string_pair
{
public:
	ANT_string_pair normalized;
	char normalized_buf[MAX_TERM_LENGTH];
	ANT_parser_token_type type;

public:
	ANT_parser_token() { normalized.start = normalized_buf; }
	ANT_string_pair* normalized_pair(void) { return normalized.length() ? &normalized : this; } 	// Get a string pair which contains the normalized form of this token, or the original form if there is no normalized form.
};

#endif
