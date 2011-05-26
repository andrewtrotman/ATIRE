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

/*
	class ANT_PARSER_TOKEN
	---------------------
*/
class ANT_parser_token : public ANT_string_pair
{
public:
	char normalized[1024];
	size_t normalized_length;
};

#endif
