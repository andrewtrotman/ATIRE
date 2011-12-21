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
	ATIRE_engine() {}
	virtual ~ATIRE_engine() {}

	virtual long load_index(char *new_index, char *new_doclist, char **old_index, char **old_doclist) = 0;
	virtual long describe_index(char **old_index, char **old_doclist, long long *documents, long long *terms, long long *length_of_longest_document) = 0;
	virtual char *search(char *query, long long top_of_page, long long page_length) = 0;
	virtual char *get_document(char *document_buffer, long long *current_document_length, long long id) = 0;
} ;

#endif /* ATIRE_ENGINE_H_ */
