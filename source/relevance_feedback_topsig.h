/*
	RELEVANCE_FEEDBACK_TOPSIG.H
	---------------------------
*/
#ifndef RELEVANCE_FEEDBACK_TOPSIG_H_
#define RELEVANCE_FEEDBACK_TOPSIG_H_

#include "relevance_feedback.h"

class ANT_index_document_topsig;

/*
	class ANT_RELEVANCE_FEEDBACK_TOPSIG
	-----------------------------------
*/
class ANT_relevance_feedback_topsig : public ANT_relevance_feedback
{
private:
	long width;												// width of the signature
	double density;											// proportion of bits that should be set
	long long collection_length_in_terms;					// the size of the document collection (measured in words)
	long stopword_mode;										// what to stop when indexing (i.e. no tags or numbers)
	ANT_index_document_topsig *global_stats;				// pointer to the object that has details of term and document frequencies in the collection
	double *feedback_vector;								// the array that holds the weighted signature

protected:
	virtual void add_to_index(char *document);

public:
	ANT_relevance_feedback_topsig(ANT_search_engine *engine);
	virtual ~ANT_relevance_feedback_topsig();

	virtual ANT_memory_index_one_node **feedback(ANT_search_engine_result *result, ANT_query *query, long documents_to_examine, long terms_wanted, long *terms_found);
	virtual ANT_memory_index_one_node **feedback(char *document, long terms_wanted, long *terms_found);

	virtual double *feedback(ANT_search_engine_result *result, long documents_to_examine);
	void set_topsig_parameters(long width, double density, long long collection_length_in_terms, ANT_index_document_topsig *global_stats);
} ;

#endif /* RELEVANCE_FEEDBACK_TOPSIG_H_ */
