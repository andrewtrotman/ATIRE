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
number_of_measures = 4;
measure_to_use = number_of_measures - 1;		// should be a measure[] that is NULL

measure = new ANT_readability*[number_of_measures];
measure[NONE] = new ANT_readability_none();
measure[DALE_CHALL] = new ANT_readability_dale_chall();
measure[TAG_WEIGHTING] = new ANT_readability_TAG_WEIGHTING();
measure[END_OF_LIST] = NULL;//new ANT_readability;

current_file = NULL;
}

/*
	ANT_READABILITY_FACTORY::~ANT_READABILITY_FACTORY()
	---------------------------------------------------
*/
ANT_readability_factory::~ANT_readability_factory()
{
long current;

for (current = 0; current < number_of_measures; current++)
	if (measure[current] != NULL)
		delete measure[current];

delete [] measure;
}

/*
	READABILITY_FACTORY::GET_NEXT_TOKEN()
	-------------------------------------
*/
ANT_parser_token *ANT_readability_factory::get_next_token()
{
ANT_parser_token *token = parser->get_next_token();

/*
	Starting with >, skip it
*/
if (token != NULL && *token->start == '>' && token->string_length == 1)
	token = parser->get_next_token();

/*
	Are we doing readability?
*/
if (measure[measure_to_use] == NULL)
	return token;

/*
	Pass it on to the given measure (which might be ANT_readability_none)
*/
measure[measure_to_use]->handle_token(token);

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
	{
	token->string_length--;
	token->normalized.string_length--;
	}

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
measure_to_use = what_measure;
}

/*
	READABILITY_FACTORY::SET_CURRENT_FILE()
	----------------------------------
*/
void ANT_readability_factory::set_current_file(	ANT_directory_iterator_object* object)
{
current_file = object;
}

/*
	READABILITY_FACTORY::HANDLE_NODE()
	----------------------------------
*/
void ANT_readability_factory::handle_node(ANT_memory_indexer_node *node)
{
if (measure[measure_to_use] != NULL)
	measure[measure_to_use]->handle_node(node);
}

/*
	READABILITY_FACTORY::HANDLE_TAG()
	----------------------------------
*/
void ANT_readability_factory::handle_tag(ANT_parser_token *token, long tag_open)
{
if (measure[measure_to_use] != NULL)
	measure[measure_to_use]->handle_tag(token, tag_open, parser);
}

/*
	READABILITY_FACTORY::INDEX()
	----------------------------
*/
void ANT_readability_factory::index(ANT_memory_indexer *index, long long doc)
{
if (measure[measure_to_use] != NULL)
	measure[measure_to_use]->index(index, doc, current_file);
}
