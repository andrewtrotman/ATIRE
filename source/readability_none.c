/*
	READABILITY_NONE.C
	------------------
*/
#include "readability_none.h"

/*
	ANT_READABILITY_NONE::GET_NEXT_TOKEN()
	--------------------------------------
*/
ANT_string_pair *ANT_readability_none::get_next_token()
{
return parser->get_next_token();
}

/*
	ANT_READABILITY_NONE::SET_DOCUMENT()
	------------------------------------
*/
void ANT_readability_none::set_document(unsigned char *document)
{
parser->set_document(document);
}

/*
	ANT_READABILITY_NONE::SCORE()
	-----------------------------
*/
long ANT_readability_none::score()
{
return 0;
}

/*
	ANT_READABILITY_NONE::ADD_NODE()
	--------------------------------
*/
void ANT_readability_none::add_node(ANT_memory_index_hash_node *token)
{
}
