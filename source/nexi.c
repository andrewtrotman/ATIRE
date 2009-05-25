/*
	NEXI.C
	------
*/
#include "nexi.h"
#include "ctypes.h"
#include "parser.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_NEXI::ISPART()
	------------------
*/
long ANT_NEXI::ispart(char *from, long length, char next)
{
if (length == 0)
	return TRUE;				/* all tokens must be at least one char in length */
else if (length == 1)
	if (*from == '.')
		return FALSE;
	else if (*from == '(')
		return FALSE;
	else if (*from == '|')
		return FALSE;
	else if (*from == ')')
		return FALSE;
	else if (ANT_parser::isXMLnamestartchar(*from))
		{
		if (ANT_parser::isXMLnamechar(next))
			return TRUE;
		else
			return FALSE;
		}
	else if (*from == '/')
		{
		if (next == '/')
			return TRUE;
		else
			return FALSE;
		}
	else
		return FALSE;
else
	{
	if (ANT_parser::isXMLnamestartchar(*from))
		if (ANT_parser::isXMLnamechar(next))
			return TRUE;
		else
			return FALSE;
	else if (ANT_isalnum(*from))
		if (ANT_isalnum(next))
			return TRUE;
		else
			return FALSE;
	else
		return FALSE;
	}
}		

/*
	ANT_NEXI::GET_NEXT_TOKEN()
	--------------------------
*/
ANT_string_pair *ANT_NEXI::get_next_token(void)
{
long length;

length = 0;

if (*at != '\0')
	while (*(at + length) != '\0' && ispart(at, length, *(at + length)))
		length++;

token.start = at;
token.string_length = length;
at += length;
return &token;
}

/*
	ANT_NEXI::READ_PATH()
	---------------------
*/
void ANT_NEXI::read_path(ANT_string_pair *path)
{
long more;

path->start = token.start;
more = TRUE;
do
	{
	get_next_token();
	if (strchr("(|)", token[0]))
		continue;
	else if (ANT_parser::isXMLnamestartchar(token[0]))
		continue;
	else
		more = FALSE;
	}
while (more);

/*
	Convert the whole path into a single 
*/
path->string_length = token.start - path->start;
}

/*
	ANT_NEXI::PARSE_ERROR()
	-----------------------
*/
void ANT_NEXI::parse_error(char *message)
{
printf("NEXI ERROR, column %ld: %s\n", (long)(at - string), message);
successful_parse = FALSE;
}

/*
	ANT_NEXI::ABOUT()
	-----------------
*/
long ANT_NEXI::about(void)
{
ANT_string_pair path;

get_next_token();
if (token[0] != '(')
	parse_error("Expected '('");
get_next_token();						// prime the read_path method with the first token in the path
read_path(&path);
if (token[0] != ',')
	parse_error("Expected ','");
read_CO(&path);
if (token[0] != ')')
	parse_error("Expected ')'");

return 0;
}

/*
	ANT_NEXI::READ_OPERATOR()
	-------------------------
*/
void ANT_NEXI::read_operator(void)
{
while (strchr("<>=", token[0]) != NULL)
	get_next_token();
}

/*
	ANT_NEXI::NUMBERS()
	-------------------
*/
long ANT_NEXI::numbers(void)
{
ANT_string_pair path;

read_path(&path);
read_operator();
read_CO(&path);

return 0;
}

/*
	ANT_NEXI::READ_CO()
	-------------------
*/
long ANT_NEXI::read_CO(ANT_string_pair *path)
{
ANT_string_pair start;

start = *get_next_token();
while (ANT_isalnum(token[0]))
	get_next_token();

return 0;
}

/*
	ANT_NEXI::READ_CAS()
	--------------------
*/
long ANT_NEXI::read_CAS(void)
{
ANT_string_pair path;
long parsing_result = FALSE;

read_path(&path);
if (token[0] == '[')
	{
	get_next_token();
	do
		{
		if (token.strcmp("about") == 0)
			about();
		else if (token[0] == '.')
			numbers();
		else
			parse_error("Expected 'about()' clause or numeric expression");
		get_next_token();
		}
	while (token.strcmp("or") == 0 || token.strcmp("and") == 0);
	}
else
	parse_error("Expected '['");

return parsing_result;
}

/*
	ANT_NEXI::PARSE()
	-----------------
*/
void ANT_NEXI::parse(char *expression)
{
successful_parse = TRUE;
at = string = expression;

get_next_token();

if (token.strcmp("//") == 0)		// we're a CAS query
	read_CAS();
else
	read_CO(NULL);
}

/*
	MAIN()
	------
*/
int main(void)
{
ANT_NEXI nexi_parser;
char buffer[1024];

for(;;)
	{
	printf("]");
	gets(buffer);
	if (strcmp(buffer, ".quit") == 0)
		break;
	nexi_parser.parse(buffer);
	puts("");
	}

return 0;
}

