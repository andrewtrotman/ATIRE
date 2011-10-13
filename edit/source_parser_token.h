/*
	SOURCE_PARSER_TOKEN.H
	---------------------
*/
#ifndef SOURCE_PARSER_TOKEN_H_
#define SOURCE_PARSER_TOKEN_H_

/*
	class ANT_SOURCE_PARSER_TOKEN
	-----------------------------
*/
class ANT_source_parser_token
{
friend class ANT_source_parser;
friend class ANT_source_parser_c;

public:
	enum { UNKNOWN, TAB, SPACE, IDENTIFIER, NUMBER, STRING, CHARACTER, RESERVED_WORD, LINE_COMMENT, OPEN_BLOCK_COMMENT, BLOCK_COMMENT, CLOSE_BLOCK_COMMENT, PREPROCESSOR_DIRECTIVE, KNOWN_ERROR }; // token types
	enum { NONE = 0, ATTRIBUTE_BLOCK_COMMENT = 1, ATTRIBUTE_UNICODE = 2 } ;		// token attributes (for example, is it part of a block comment?)

private:
	char *string_start;
	long long string_length;
	long long string_type;
	long long string_attributes;

public:
	char *string(void) { return string_start; }
	long long length(void) { return string_length; }
	long long type(void) { return string_type; }
	long long attributes(void) { return string_attributes; }
} ;

#endif /* SOURCE_PARSER_TOKEN_H_ */
