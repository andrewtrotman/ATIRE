/*
	RELEVANCE_FEEDBACK.H
	--------------------
*/
#ifndef RELEVANCE_FEEDBACK_H_
#define RELEVANCE_FEEDBACK_H_

class ANT_memory;
class ANT_memory_index;
class ANT_memory_index_one;
class ANT_memory_index_one_node;
class ANT_search_engine;
class ANT_search_engine_result;
class ANT_parser;

/*
	class ANT_RELEVANCE_FEEDBACK
	----------------------------
*/
class ANT_relevance_feedback
{
protected:
	ANT_memory *memory;
	ANT_search_engine *search_engine;
	ANT_memory_index *indexer;
	ANT_memory_index_one *one;
	ANT_parser *parser;
	char *document_buffer;

	double feedback_vector;					// this is where we sum the bit-string weights

protected:
	/*
		Allocate neceessary structures and set them ready for use (clean them up if already in use).
	*/
	virtual void rewind(void);

	/*
		Add a single document to the in-memory index
	*/
	virtual void add_to_index(char *document);

	/*
		Add the top documents_to_examine documents from the results list to the in-memory index
	*/
	virtual void populate(ANT_search_engine_result *result, long documents_to_examine);
	virtual void populate(char *document);

public:
	ANT_relevance_feedback(ANT_search_engine *engine);
	virtual ~ANT_relevance_feedback();

	virtual ANT_memory_index_one_node **feedback(ANT_search_engine_result *result, long documents_to_examine, long terms_wanted, long *terms_found) = 0;
	virtual ANT_memory_index_one_node **feedback(char *document, long terms_wanted, long *terms_found) = 0;
} ;

#endif /* RELEVANCE_FEEDBACK_H_ */

