/*
	READABILITY_FACTORY.C
	---------------------
*/
#include "readability.h"
#include "readability_factory.h"
#include "readability_none.h"
#include "readability_dale_chall.h"
#include "readability_tag_weighting.h"

/*
	ANT_READABILITY_FACTORY::ANT_READABILITY_FACTORY()
	--------------------------------------------------
*/
ANT_readability_factory::ANT_readability_factory() 
{
measures_to_use = 0;
number_of_measures = 5;

measure = new ANT_readability*[number_of_measures];
measure[0] = new ANT_readability_none();
measure[1] = new ANT_readability_dale_chall();
measure[2] = NULL;//new ANT_readability;
measure[3] = NULL; // not existing
measure[4] = new ANT_readability_TAG_WEIGHTING();
}

/*
	ANT_READABILITY_FACTORY::~ANT_READABILITY_FACTORY()
	---------------------------------------------------
*/
ANT_readability_factory::~ANT_readability_factory()
{
for (int i = 0; i < number_of_measures; ++i)
	if (measure[i] != NULL)
		delete measure[i];

delete [] measure;
}

/*
	READABILITY_FACTORY::GET_NEXT_TOKEN()
	-------------------------------------
*/
ANT_parser_token *ANT_readability_factory::get_next_token()
{
long which;
ANT_parser_token *token = parser->get_next_token();

/*
	Starting with >, skip it
*/
if (token != NULL && *token->start == '>' && token->string_length == 1)
	token = parser->get_next_token();

if (measures_to_use == 0)
	return token;

/*
	Each measure we're using should now handle the token
*/
if (measures_to_use == TAG_WEIGHTING)
	{
	if (token != NULL && token->type != TT_TAG_CLOSE)
		measure[TAG_WEIGHTING]->handle_token(token);
	}
else
	for (which = 0; which < number_of_measures; which++)
		if ((which & measures_to_use) != 0)
			measure[which]->handle_token(token);

/*
	After we've handled all the tokens, clean it up and pass it on
*/
if (token == NULL)
	return token;

/*
	If it starts with a sentence end, then it's only sentence ends, so we
	need to return a new token entirely
*/
if (ANT_parser_readability::issentenceend(*token->start))
	return get_next_token();

/*
	Starting with < means it's a tag, so remove the < and pass it back
*/
if (*token->start == '<')
	{
	token->start++;
	token->string_length--;
	return token;
	}

/*
	Strip the sentence ends off the end
*/
while (ANT_parser_readability::issentenceend(token->start[token->length() - 1]))
	token->string_length--;

return token;
}

/*
	READABILITY_FACTORY::SET_DOCUMENT()
	-----------------------------------
*/
void ANT_readability_factory::set_document(unsigned char *document)
{
parser->set_document(document);
}

/*
	READABILITY_FACTORY::SET_PARSER()
	---------------------------------
*/
void ANT_readability_factory::set_parser(ANT_parser *parser)
{
this->parser = parser;
}

/*
	READABILITY_FACTORY::SET_MEASURE()
	----------------------------------
*/
void ANT_readability_factory::set_measure(unsigned long what_measure)
{
measures_to_use = what_measure;
}

/*
	READABILITY_FACTORY::HANDLE_NODE()
	----------------------------------
*/
void ANT_readability_factory::handle_node(ANT_memory_indexer_node *node)
{
long which;

if (measures_to_use == 0)
	return;

for (which = 0; which < number_of_measures; which++)
	if ((which & measures_to_use) != 0)
		measure[which]->handle_node(node);
}

/*
	READABILITY_FACTORY::HANDLE_TAG()
	----------------------------------
*/
void ANT_readability_factory::handle_tag(ANT_string_pair* token, long tag_open)
{
if ((measures_to_use & TAG_WEIGHTING) != 0)
	measure[TAG_WEIGHTING]->handle_tag(token, tag_open, parser);
}

/*
	READABILITY_FACTORY::INDEX()
	----------------------------
*/
void ANT_readability_factory::index(ANT_memory_indexer *index, long long doc)
{
long which;

if (measures_to_use == 0)
	return;

for (which = 0; which < number_of_measures; which++)
	if ((which & measures_to_use) != 0)
		measure[which]->index(index, doc);
}
