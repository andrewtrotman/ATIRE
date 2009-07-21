/*
	READABILITY_FACTORY.C
	---------------------
*/
#include "readability_factory.h"
#include "readability_none.h"
#include "readability_dale_chall.h"

/*
	READABILITY_FACTORY::READABILITY_FACTORY()
	-------------------------------------
*/
ANT_readability_factory::ANT_readability_factory()
{
measure = 0;
}

/*
	READABILITY_FACTORY::~READABILITY_FACTORY()
	-------------------------------------
*/
ANT_readability_factory::~ANT_readability_factory()
{
if (measure) delete measure;
}

/*
	READABILITY_FACTORY::GET_NEXT_TOKEN()
	-------------------------------------
*/
ANT_string_pair *ANT_readability_factory::get_next_token()
{
return measure->get_next_token();
}

/*
	READABILITY_FACTORY::SET_DOCUMENT()
	-----------------------------------
*/
void ANT_readability_factory::set_document(unsigned char *document)
{
measure->set_document(document);
}

/*
	READABILITY_FACTORY::SCORE()
	----------------------------
*/
long ANT_readability_factory::score()
{
return measure->score();
}

/*
	READABILITY_FACTORY::SET_MEASURE()
	----------------------------------
*/
void ANT_readability_factory::set_measure(unsigned long what_measure)
{
switch (what_measure)
	{
	case NONE:
		measure = new ANT_readability_none();
		break;
	case DALE_CHALL:
		measure = new ANT_readability_dale_chall();
		break;
	default:
		measure = new ANT_readability_none();
		break; // shouldn't happen
	}
}

/*
	READABILITY_FACTORY::SET_PARSER()
	---------------------------------
*/
void ANT_readability_factory::set_parser(ANT_parser *parser)
{
measure->set_parser(parser);
}

/*
	READABILITY_FACTORY::ADD_NODE()
	-------------------------------
*/
void ANT_readability_factory::add_node(ANT_memory_index_hash_node *node)
{
measure->add_node(node);
}

/*
	READABILITY_FACTORY::INDEX()
	----------------------------
*/
void ANT_readability_factory::index(ANT_memory_index *index)
{
measure->index(index);
}
