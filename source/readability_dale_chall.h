/*
	READABILITY_DALE_CHALL.H
	------------------------
*/

#ifndef READABILITY_DALE_CHALL_H_
#define READABILITY_DALE_CHALL_H_

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
	ANT_memory_indexer_node *node;
	unsigned long istitle;
	};

private:
	static ANT_string_pair measure_name;
	size_t number_of_sentences;
	size_t number_of_words;
	size_t number_of_unfamiliar_words;
	word *words_encountered;
	unsigned long last_was_title; // whether the last word we passed on was title
	unsigned long size;

private:
	static int word_cmp(const void *a, const void *b);

protected:
	long score(void);
	void handle_token(ANT_parser_token *token);
	void handle_node(ANT_memory_indexer_node *node);

public:
	ANT_readability_dale_chall();
	virtual ~ANT_readability_dale_chall();
	virtual void index(ANT_memory_indexer *index, long long doc);
} ;

#endif  /* READABILITY_DALE_CHALL_H_ */
