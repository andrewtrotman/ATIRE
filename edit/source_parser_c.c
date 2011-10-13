/*
	SOURCE_PARSER_C.C
	-----------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/ctypes.h"
#include "source_parser_c.h"

/*
	Sorted list of C/C++ reserved words
*/
static char *c_reserved_words[] =
{
"alignas",
"alignof",
"and",
"and_eq",
"asm",
"auto",
"bitand",
"bitor",
"bool",
"break",
"case",
"catch",
"char",
"char16_t",
"char32_t",
"class",
"compl",
"const",
"const_cast",
"constexpr",
"continue",
"decltype",
"default",
"delete",
"double",
"dynamic_cast",
"else",
"enum",
"explicit",
"export",
"extern",
"false",
"float",
"for",
"friend",
"goto",
"if",
"inline",
"int",
"long",
"mutable",
"namespace",
"new",
"noexcept",
"not",
"not_eq",
"nullptr",
"operator",
"or",
"or_eq",
"private",
"protected",
"public",
"register",
"reinterpret_cast",
"return",
"short",
"signed",
"sizeof",
"static",
"static_assert",
"static_cast",
"struct",
"switch",
"template",
"this",
"thread_local",
"throw",
"true",
"try",
"typedef",
"typeid",
"typename",
"union",
"unsigned",
"using",
"virtual",
"void",
"volatile",
"wchar_t",
"while",
"xor",
"xor_eq"
} ;
static long long c_reserved_words_size = sizeof(c_reserved_words) / sizeof(*c_reserved_words);

/*
	Sorted list of C/C++ preprocessor reserved words
*/
static char *c_preprocessor_reserved_words[] =
{
"define",
"elif",
"else",
"endif",
"error",
"if",
"ifdef",
"ifndef",
"include",
"line",
"pragma",
"undef",
"warning"
} ;
static long long c_preprocessor_reserved_words_size = sizeof(c_preprocessor_reserved_words) / sizeof(*c_preprocessor_reserved_words);


/*
	ANT_SOURCE_PARSER_C::RESERVED_WORD_CMP()
	----------------------------------------
*/
int ANT_source_parser_c::reserved_word_cmp(const void *a, const void *b)
{
ANT_source_parser_token *token = (ANT_source_parser_token *)a;
char **word = (char **)b;
int cmp;

if ((cmp = strncmp(token->string(), *word, token->length())) == 0)
	if (strlen(*word) == token->length())
		return 0;
	else
		return -1;
else
	return cmp;
}

/*
	ANT_SOURCE_PARSER_C::NEXT()
	---------------------------
*/
ANT_source_parser_token *ANT_source_parser_c::next(void)
{
char *start, *from;
int (*ANT_is)(int);

if ((start = at) == NULL)
	return NULL;

if (*at == '\0')
	return NULL;

if (*at == '\t')
	{
	token.string_type = ANT_source_parser_token::TAB;
	at++;
	}
else if (ANT_isspace(*at))
	{
	token.string_type = ANT_source_parser_token::SPACE;
	++at;
	while (ANT_isspace(*at) && *at != '\t')
		at++;
	}
else if (ANT_isalpha(*at))
	{
	token.string_type = ANT_source_parser_token::IDENTIFIER;
	at++;
	while (ANT_isalnum(*at) || *at == '_')
		at++;

	/*
		Check to see if its a reserved word.
	*/
	token.string_start = start;
	token.string_length = at - start;
	if ((bsearch(&token, c_reserved_words, c_reserved_words_size, sizeof(*c_reserved_words), reserved_word_cmp)) != NULL)
		token.string_type = ANT_source_parser_token::RESERVED_WORD;
	}
else if (ANT_isdigit(*at) || *at == '.')
	{
	token.string_type = ANT_source_parser_token::NUMBER;
	if (*at == '0' && (*(at + 1) == 'x' || *(at + 1) == 'X'))		// for 0x00
		{
		at++;
		ANT_is = ANT_isxdigit;
		}
	else
		ANT_is = ANT_isdigit;

	at++;
	while (ANT_is(*at) || *at == '.')
		at++;
	}
else if (*at == '"')
	{
	if (in_block_comment)
		token.string_type = ANT_source_parser_token::BLOCK_COMMENT;
	else
		{
		token.string_type = ANT_source_parser_token::STRING;
		at++;
		while (*at != '"')
			{
			if (*at == '\0')
				break;
			else if (*at == '\\')
				if (*(at + 1) == '\0')
					break;
				else
					at++;
			at++;
			}
		}
	if (*at == '"')
		at++;
	}
else if (*at == '\'')
	{
	if (in_block_comment)
		token.string_type = ANT_source_parser_token::BLOCK_COMMENT;
	else
		{
		token.string_type = ANT_source_parser_token::CHARACTER;
		at++;
		while (*at != '\'')
			{
			if (*at == '\0')
				break;
			else if (*at == '\\')
				if (*(at + 1) == '\0')	
					break;
				else
					at++;
			at++;
			}
		}
	if (*at == '\'')
		at++;
	}
else if (*at == '/' && *(at + 1) == '*')
	{
	token.string_type = ANT_source_parser_token::OPEN_BLOCK_COMMENT;
	in_block_comment = true;
	at += 2;
	}
else if (*at == '*' && *(at + 1) == '/')
	{
	token.string_type = ANT_source_parser_token::CLOSE_BLOCK_COMMENT;
	at += 2;
	}
else if (*at == '/' && *(at + 1) == '/')
	{
	token.string_type = ANT_source_parser_token::LINE_COMMENT;
	while (*at != '\0')
		at++;
	}
else if (*at == '#')
	{
	token.string_type = ANT_source_parser_token::PREPROCESSOR_DIRECTIVE;
	if ((*at + 1) == '#')
		at += 2;				// for ##
	else
		{
		at++;
		while (ANT_isspace(*at))
			at++;
		from = at;
		while (ANT_isalpha(*at))
			at++;
		token.string_start = from;
		token.string_length = at - from;

		if ((bsearch(&token, c_preprocessor_reserved_words, c_preprocessor_reserved_words_size, sizeof(*c_preprocessor_reserved_words), reserved_word_cmp)) != NULL)
			token.string_type = ANT_source_parser_token::PREPROCESSOR_DIRECTIVE;
		else
			token.string_type = ANT_source_parser_token::KNOWN_ERROR;
		}
	}
else
	{
	token.string_type = ANT_source_parser_token::UNKNOWN;
	at++;
	}

token.string_start = start;
token.string_length = at - start;

token.string_attributes = ANT_source_parser_token::NONE;
if (in_block_comment)
	{
	token.string_attributes = ANT_source_parser_token::ATTRIBUTE_BLOCK_COMMENT;
	if (token.type() == ANT_source_parser_token::CLOSE_BLOCK_COMMENT)
		in_block_comment = false;
	}

for (start = token.string_start; start < at; start++)
	if ((*start & 0x80) != 0)
		token.string_attributes |= ANT_source_parser_token::ATTRIBUTE_UNICODE;

return &token;
}
