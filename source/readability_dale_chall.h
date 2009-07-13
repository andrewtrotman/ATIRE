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
	//unsigned long size; // maximum size of list of words encountered
	word *words_encountered;
	unsigned long last_was_title; // whether the last word we passed on was title
	// largest document in wiki is 65003
	unsigned long size;

private:
	static int word_cmp(const void *a, const void *b);

public:
	ANT_readability_dale_chall();
	virtual ~ANT_readability_dale_chall();
	
	ANT_string_pair *get_next_token(void);
	long score(void);
	void set_document(unsigned char *document);
	void add_node(ANT_memory_index_hash_node *node);
	void index(ANT_memory_index *index);
} ;

#endif __READABILITY_DALE_CHALL_H__
