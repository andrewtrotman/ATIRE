/*
	READABILITY_DALE_CHALL.C
	------------------------
*/
#include "readability_dale_chall.h"

/*
	ANT_READABILITY_DALE_CHALL::ANT_READABILITY_DALE_CHALL()
	--------------------------------------------------------
*/
ANT_readability_dale_chall::ANT_readability_dale_chall(ANT_parser *parser) : ANT_readability(parser)
{
	this->number_of_words = this->number_of_sentences = 0;
	// TODO: Reset words in document stuff
}

/*
	ANT_READABILITY_DALE_CHALL::GET_NEXT_TOKEN()
	--------------------------------------------
*/
ANT_string_pair *ANT_readability_dale_chall::get_next_token()
{
	ANT_string_pair *token = this->parser->get_next_token();

	// TODO: Use the token received to adjust the score
	// 		 If we haven't got an indexable token, keep going until we do
	
	return token;
}

/*
	ANT_READABILITY_DALE_CHALL::SCORE()
	-----------------------------------
*/
long ANT_readability_dale_chall::score()
{
	// TODO: Calculate actual score
	//
	// (0.049 * (nw / ns)) + (15.79 * (nu / nw)) + 3.6365
	return 0;
}

/*
	ANT_READABILITY_DALE_CHALL::SET_DOCUMENT()
	------------------------------------------
*/
void ANT_readability_dale_chall::set_document(unsigned char *document)
{
	this->parser->set_document(document);
	this->number_of_sentences = this->number_of_words = 0;
	// TODO: Reset the word lists for this document
}
