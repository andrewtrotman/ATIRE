/*
	ATIRE_BROKE.H
	-------------
*/
#ifndef ATIRE_BROKE_H_
#define ATIRE_BROKE_H_

#include "atire_engine.h"

class ATIRE_broke_engine;
class ATIRE_broker_param_block;
class ATIRE_engine_result_set;

/*
	class ATIRE_BROKE
	-----------------
*/
class ATIRE_broke : public ATIRE_engine
{
private:
	ATIRE_broke_engine **search_engine;
	ATIRE_engine_result_set *results_list;

public:
	ATIRE_broke(ATIRE_broker_param_block *params);
	virtual ~ATIRE_broke();

	virtual long load_index(char *new_index, char *new_doclist, char **old_index, char **old_doclist);
	virtual long describe_index(char **old_index, char **old_doclist, long long *documents, long long *terms, long long *length_of_longest_document);
	virtual char *search(char *query, long long top_of_page, long long page_length);
	virtual char *get_document(char *document_buffer, long long *current_document_length, long long id);
} ;


#endif /* ATIRE_BROKE_H_ */
