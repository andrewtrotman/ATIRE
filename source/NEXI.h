/*
	NEXI.H
	------
*/
#ifndef __NEXI_H__
#define __NEXI_H__

#include "string_pair.h"

class ANT_NEXI_term;
class ANT_string_term;

/*
	class ANT_NEXI
	--------------
*/
class ANT_NEXI 
{
private:
	long successful_parse;
	ANT_string_pair token;
	char *string, *at;

private:
	ANT_NEXI_term *get_NEXI_term(ANT_NEXI_term *parent, ANT_string_pair *tag, ANT_string_pair *term, long weight);
	ANT_NEXI_term *duplicate_path_chain(ANT_NEXI_term *start, ANT_NEXI_term **end_of_chain);
	long ispart(char *from, long length, char next);
	ANT_string_pair *get_next_token(void);
	void read_path(ANT_string_pair *path);
	void parse_error(char *message);
	ANT_NEXI_term *about(void);
	long read_phrase(ANT_string_pair *string);
	long read_phraseless_term(ANT_string_pair *term);
	long read_term(ANT_string_pair *term);
	void read_operator(void);
	ANT_NEXI_term *numbers(void);
	ANT_NEXI_term *read_CO(ANT_string_pair *path, ANT_string_pair *terms);
	ANT_NEXI_term *read_CAS(void);

public:
	ANT_NEXI_term *parse(char *expression);
	long get_success_state(void) { return successful_parse; }
} ;

#endif __NEXI_H__

