/*
	NEXI.H
	------
*/
#ifndef __NEXI_H__
#define __NEXI_H__

#include "string_pair.h"

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
	long ispart(char *from, long length, char next);
	ANT_string_pair *get_next_token(void);
	void read_path(ANT_string_pair *path);
	void parse_error(char *message);
	long about(void);
	long read_phrase(ANT_string_pair *string);
	long read_phraseless_term(ANT_string_pair *term);
	long read_term(ANT_string_pair *term);
	void read_operator(void);
	long numbers(void);
	void read_CO(ANT_string_pair *path, ANT_string_pair *terms);
	long read_CAS(void);

public:
	void parse(char *expression);
} ;


#endif __NEXI_H__
