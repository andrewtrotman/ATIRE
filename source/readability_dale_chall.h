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
	virtual ~ANT_readability_dale_chall() {};
	
	ANT_string_pair *get_next_token();
	void set_document(unsigned char *);
	long score();

private:
	long number_of_sentences;
	long number_of_words;
};

#endif __READABILITY_DALE_CHALL_H__
