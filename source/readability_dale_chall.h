/*
	READABILITY_DALE_CHALL.H
	------------------------
*/

#ifndef __READABILITY_DALE_CHALL_H__
#define __READABILITY_DALE_CHALL_H__

#include "readability.h"
#include "parser_readability.h"

class ANT_string_pair;

/*
	class ANT_READABILITY_DALE_CHALL
	--------------------------------
*/
class ANT_readability_dale_chall : public ANT_readability
{
private:
	struct word
	{
	ANT_memory_index_hash_node *node;
	unsigned long istitle;
	};

private:
	ANT_string_pair *measure_name;
	unsigned long number_of_sentences;
	unsigned long number_of_words;
	unsigned long number_of_unfamiliar_words;
	word *words_encountered;
	unsigned long last_was_title; // whether the last word we passed on was title
	unsigned long size;

private:
	static int word_cmp(const void *a, const void *b);

protected:
	long score(void);
	void handle_token(ANT_string_pair *token);
	void handle_node(ANT_memory_index_hash_node *node);

public:
	ANT_readability_dale_chall();
	virtual ~ANT_readability_dale_chall() {};
	
	void index(ANT_memory_index *index);
} ;

#endif __READABILITY_DALE_CHALL_H__
