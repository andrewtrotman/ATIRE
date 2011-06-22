/*
	ATIRE_BROKE.H
	-------------
*/
#ifndef ATIRE_BROKE_H_
#define ATIRE_BROKE_H_

#include "atire_engine.h"

class ATIRE_broke_engine;
class ATIRE_broker_param_block;

/*
	class ATIRE_BROKE
	-----------------
*/
class ATIRE_broke : public ATIRE_engine
{
private:
	ATIRE_broke_engine **search_engine;

public:
	ATIRE_broke(ATIRE_broker_param_block *params);
	virtual ~ATIRE_broke();

	virtual long load_index(char *new_index, char *new_doclist, char **old_index, char **old_doclist);
	virtual long describe_index(char **old_index, char **old_doclist);
	virtual long search(char *query, long long top_of_page, long long page_length);
} ;


#endif /* ATIRE_BROKE_H_ */
