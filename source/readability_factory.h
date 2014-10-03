/*
	READABILITY_FACTORY.H
	---------------------
*/
#ifndef READABILITY_FACTORY_H_
#define READABILITY_FACTORY_H_

#include "readability.h"

/*
	class ANT_READABILITY_FACTORY
	-----------------------------
*/
class ANT_readability_factory : public ANT_readability
{
public:
	enum {
		NONE,
		DALE_CHALL,
		TAG_WEIGHTING,
		END_OF_LIST
		};

private:
	long number_of_measures;
	ANT_readability **measure;
	unsigned long measure_to_use;
	ANT_parser *parser;
	ANT_directory_iterator_object *current_file;

public:
	ANT_readability_factory();
	virtual ~ANT_readability_factory();
	
	ANT_parser_token *get_next_token(void);
	virtual void handle_node(ANT_memory_indexer_node *node);
	using ANT_readability::handle_tag;
	virtual void handle_tag(ANT_parser_token *token, long tag_open);
	void index(ANT_memory_indexer *index, long long doc);

	void set_document(unsigned char *document);
	void set_measure(unsigned long value);

	void set_parser(ANT_parser *parser);

	void set_current_file(ANT_directory_iterator_object *object);
};


#endif  /* READABILITY_FACTORY_H_ */
