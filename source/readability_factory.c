/*
	READABILITY_FACTORY.C
	---------------------
*/
#include "readability_factory.h"
#include "readability_factory_measure.h"
#include "readability_none.h"
#include "readability_dale_chall.h"

/*
	static
	------
	Declaration of an object of each of the known readability types.
	Pointers to these will be used below as objects to calculate readability.
*/
static ANT_readability_none none;
static ANT_readability_dale_chall dale_chall;

/*
	ANT_readability_factory::measure[]
	----------------------------------
	The known readability measures that can be used
*/
ANT_readability_factory_measure ANT_readability_factory::measure[] =
{
{NONE, &none},
{DALE_CHALL, &dale_chall}
};

long ANT_readability_factory::number_of_measures = sizeof(ANT_readability_factory::measure) / sizeof(*ANT_readability_factory::measure);

/*
	READABILITY_FACTORY::GET_NEXT_TOKEN()
	-------------------------------------
*/
ANT_string_pair *ANT_readability_factory::get_next_token()
{
long which;
ANT_string_pair *token = parser->get_next_token();

/*
	Each measure we're using should now handle the token
*/
for (which = 0; which < number_of_measures; which++)
	if ((measure[which].measure_id & measures_to_use) != 0)
		measure[which].measure->handle_token(token);

/*
	After we've handled all the tokens, clean it up and pass it on
*/
if (token == NULL)
	return token;

/*
	If it starts with a sentence end, then it's only sentence ends, so we
	need to retun a new token entirely
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

/*
	And finally, lowercase the whole thing
*/
return token->strlower();
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
void ANT_readability_factory::handle_node(ANT_memory_index_hash_node *node)
{
long which;

for (which = 0; which < number_of_measures; which++)
	if ((measure[which].measure_id & measures_to_use) != 0)
		measure[which].measure->handle_node(node);
}

/*
	READABILITY_FACTORY::INDEX()
	----------------------------
*/
void ANT_readability_factory::index(ANT_memory_index *index)
{
long which;

for (which = 0; which < number_of_measures; which++)
	if ((measure[which].measure_id & measures_to_use) != 0)
		measure[which].measure->index(index);
}
