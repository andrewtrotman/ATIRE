/*
	READABILITY_DALE_CHALL.H
	------------------------
*/

#ifndef __READABILITY_DALE_CHALL_H__
#define __READABILITY_DALE_CHALL_H__

#include "readability.h"

/*
	class ANT_READABILITY_DALE_CHALL
	--------------------------------
*/
class ANT_readability_dale_chall : public ANT_readability
{
public:
	ANT_readability_dale_chall();
	virtual ~ANT_readability_dale_chall();
	
	ANT_string_pair *get_next_token();
	void set_document(unsigned char *);
	long score();
	void add_token(ANT_string_pair *) {};
	void add_token(char *);

private:
	unsigned long number_of_sentences;
	unsigned long number_of_words;
	unsigned long number_of_unfamiliar_words;
	unsigned long size; // maximum size of list of words encountered
	char **words_encountered;

	static const unsigned long initial_size = 50;
};

#endif __READABILITY_DALE_CHALL_H__
