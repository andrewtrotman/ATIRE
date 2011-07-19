/*
	ATIRE_ENGINE.H
	--------------
*/
#ifndef ATIRE_ENGINE_H_
#define ATIRE_ENGINE_H_

class ATIRE_engine_param_block;

/*
	class ATIRE_ENGINE
	------------------
*/
class ATIRE_engine
{
public:
	ATIRE_engine(ATIRE_engine_param_block *params) {}
	virtual ~ATIRE_engine() {}

	virtual long load_index(char *new_index, char *new_doclist, char **old_index, char **old_doclist) = 0;
	virtual long describe_index(char **old_index, char **old_doclist, long long *documents) = 0;
	virtual char *search(char *query, long long top_of_page, long long page_length) = 0;
} ;

#endif /* ATIRE_ENGINE_H_ */
