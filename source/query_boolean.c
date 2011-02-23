/*
	QUERY_BOOLEAN.C
	---------------
*/
#include <stdio.h>
#include "ctypes.h"
#include "string_pair.h"
#include "query_boolean.h"
#include "query_parse_tree.h"
#include "query.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_QUERY_BOOLEAN::PARSE()
	--------------------------
*/
ANT_query *ANT_query_boolean::parse(ANT_query *into, char *query, long default_operator)
{
ANT_string_pair primed;
ANT_query_parse_tree *answer;

leaves = 0;
nodes_used = 0;
query_is_disjunctive = TRUE;

this->default_operator = default_operator;
error_code = ANT_query::ERROR_NONE;
this->next_character = this->query = query;

token_peek.start = "";
token_peek.string_length = 0;
get_token(&primed);		// prime the tokenizer;

answer = parse();

if (*next_character != '\0')
	error_code = ANT_query::ERROR_PREMATURE_END_OF_QUERY;

into->set_query(answer);
into->set_error(error_code);
into->set_subtype(query_is_disjunctive ? ANT_query::DISJUNCTIVE : ANT_query::CONJUNCTIVE);
into->terms_in_query = leaves;

return answer == NULL ? NULL : into;
}

/*
	ANT_QUERY_BOOLEAN::MAKE_TOKEN()
	-------------------------------
*/
ANT_string_pair *ANT_query_boolean::make_token(ANT_string_pair *token)
{
token->start = NULL;

if (*next_character == '\0')
	return NULL;

/*
	remove leading whitespace
*/
while (!(ANT_isalnum(*next_character) || *next_character == '(' || *next_character == ')' || *next_character == '\0'))
	next_character++;

if (*next_character == '\0')
	return NULL;

token->start = next_character;

if (ANT_isalpha(*next_character))				// text tokens
	do
		next_character++;
	while (ANT_isalpha(*next_character));
else if (ANT_isdigit(*next_character))			// numbers
	do
		next_character++;
	while (ANT_isdigit(*next_character));
else											// we have a special character (such as a bracket)
	next_character++;

token->string_length = next_character - token->start;

/*
	now return the completed token
*/
return token;
}

/*
	ANT_QUERY_BOOLEAN::GET_TOKEN()
	------------------------------
*/
ANT_string_pair *ANT_query_boolean::get_token(ANT_string_pair *next_token)
{
if (token_peek.start == NULL)
	return NULL;

*next_token = token_peek;
make_token(&token_peek);

return next_token;
}

/*
	ANT_QUERY_BOOLEAN::PEEK_TOKEN()
	-------------------------------
*/
ANT_string_pair *ANT_query_boolean::peek_token(void)
{
return token_peek.start == NULL ? NULL : &token_peek;
}

/*
	ANT_QUERY_BOOLEAN::PARSE()
	--------------------------
*/
ANT_query_parse_tree *ANT_query_boolean::parse(long depth)
{
ANT_string_pair token, *got;
ANT_query_parse_tree *left, *node;

if (depth > MAX_DEPTH)
	{
	error_code = ANT_query::ERROR_NESTED_TOO_DEEP;
	return NULL;
	}

got = get_token(&token);
if (got == NULL)
	return NULL;

if (got->string_length == 1 && *got->start == '(')
	left = parse(depth + 1);
else if (got->string_length == 1 && *got->start == ')')
	return NULL;
else
	{
	if ((left = new_node()) == NULL)
		{
		error_code = ANT_query::ERROR_NO_MEMORY;
		return NULL;
		}
	left->boolean_operator = ANT_query_parse_tree::LEAF_NODE;
	left->term = token;			// shallow copy (it does *not* copy the string, it re-uses the pointer)
	left->left = left->right = NULL;
	leaves++;
	}

#pragma ANT_PRAGMA_CONST_CONDITIONAL
while (1)
	{
	got = peek_token();				// the boolean operator
	if (got == NULL)
		return left;

	if (got->string_length == 1 && *got->start == ')')
		{
		get_token(&token);
		return left;
		}

	if ((node = new_node()) == NULL)
		{
		error_code = ANT_query::ERROR_NO_MEMORY;
		return NULL;
		}
	node->left = left;
	node->right = NULL;
	node->term = *got;			// shallow copy

	if (got->true_strcmp("or") == 0 || got->true_strcmp("OR") == 0)
		{
		get_token(&token);
		node->boolean_operator = ANT_query_parse_tree::BOOLEAN_OR;
		}
	else if (got->true_strcmp("xor") == 0 || got->true_strcmp("XOR") == 0)
		{
		get_token(&token);
		node->boolean_operator = ANT_query_parse_tree::BOOLEAN_XOR;
		query_is_disjunctive = FALSE;
		}
	else if (got->true_strcmp("and") == 0 || got->true_strcmp("AND") == 0)
		{
		get_token(&token);
		node->boolean_operator = ANT_query_parse_tree::BOOLEAN_AND;
		query_is_disjunctive = FALSE;
		}
	else if (got->true_strcmp("not") == 0 || got->true_strcmp("NOT") == 0)
		{
		get_token(&token);
		node->boolean_operator = ANT_query_parse_tree::BOOLEAN_NOT;
		query_is_disjunctive = FALSE;
		}
	else
		{
		node->boolean_operator = default_operator;
		if (default_operator != ANT_query_parse_tree::BOOLEAN_OR) 
			query_is_disjunctive = FALSE;
		}

	/*
		term on the right of the operator
	*/
	got = get_token(&token);
	if (got == NULL)
		{
		error_code = ANT_query::ERROR_MISSING_RIGHT_IN_SUBEXPRESSION;
		return node;			// end of input stream;
		}

	if (got->string_length == 1 && *got->start == '(')
		node->right = parse(depth + 1);
	else if (got->string_length == 1 && *got->start == ')')
		return node;
	else
		{
		if ((node->right = new_node()) == NULL)
			{
			error_code = ANT_query::ERROR_NO_MEMORY;
			return NULL;
			}
		node->right->left = node->right->right = NULL;
		node->right->boolean_operator = ANT_query_parse_tree::LEAF_NODE;
		node->right->term = *got;
		leaves++;
		}

	left = node;
	}

return node;
}
