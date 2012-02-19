/*
	RELEVANCE_FEEDBACK_TOPSIG.C
	---------------------------
*/
#include <new>
#include "relevance_feedback_topsig.h"
#include "index_document.h"
#include "memory_index.h"
#include "memory_index_one.h"
#include "index_document_topsig_signature.h"
#include "search_engine.h"
#include "parser.h"
#include "readability_factory.h"
#include "maths.h"

/*
	ANT_RELEVANCE_FEEDBACK_TOPSIG::ANT_RELEVANCE_FEEDBACK_TOPSIG()
	--------------------------------------------------------------
*/
ANT_relevance_feedback_topsig::ANT_relevance_feedback_topsig(ANT_search_engine *engine) : ANT_relevance_feedback(engine)
{
feedback_vector = NULL;
}

/*
	ANT_RELEVANCE_FEEDBACK_TOPSIG::~ANT_RELEVANCE_FEEDBACK_TOPSIG()
	---------------------------------------------------------------
*/
ANT_relevance_feedback_topsig::~ANT_relevance_feedback_topsig()
{
delete [] feedback_vector;
}

/*
	ANT_RELEVANCE_FEEDBACK_TOPSIG::SET_TOPSIG_PARAMETERS()
	------------------------------------------------------
*/
void ANT_relevance_feedback_topsig::set_topsig_parameters(long width, double density, long long collection_length_in_terms, ANT_index_document_topsig *global_stats)
{
this->width = width;
this->density = density;
this->collection_length_in_terms = collection_length_in_terms;
this->stopword_mode = ANT_memory_index::PRUNE_NUMBERS | ANT_memory_index::PRUNE_TAGS;
this->global_stats = global_stats;

delete [] feedback_vector;
feedback_vector = new double [width];
memset(feedback_vector, 0, sizeof(*feedback_vector) * width);
}

/*
	ANT_RELEVANCE_FEEDBACK_TOPSIG::ADD_TO_INDEX()
	---------------------------------------------
*/
void ANT_relevance_feedback_topsig::add_to_index(char *document)
{
unsigned long long seed = 0;
ANT_memory_indexer_node **term_list, **current;
long length, bit;
double *vector;
ANT_string_pair as_string;
ANT_memory_index_one *document_indexer;					// the object that generates the initial ANT index
ANT_index_document_topsig_signature *signature;			// the current signature we're working on
ANT_readability_factory *readability;
ANT_parser *parser;
ANT_index_document worker;

/*
	Set up a parser
*/
parser = new ANT_parser(ANT_parser::NOSEGMENTATION);
readability = new ANT_readability_factory;
readability->set_measure(ANT_readability_factory::NONE);
readability->set_parser(parser);

/*
	allocate all the necessary memory
*/
signature = new (std::nothrow) ANT_index_document_topsig_signature(width, density, stopword_mode);
document_indexer = new (std::nothrow) ANT_memory_index_one(new ANT_memory(1024 * 1024));

/*
	First pass index and get the list of terms and term counts
*/
length = worker.index_document(document_indexer, search_engine->get_stemmer(), ANT_parser::NOSEGMENTATION, readability, 0, document);
term_list = document_indexer->get_term_list();

/*
	Now walk the term list generating the signatures
*/
length = 0;
for (current = term_list; *current != NULL; current++)
	if ((seed = signature->add_term(global_stats, &((*current)->string), (*current)->term_frequency, length, collection_length_in_terms)) != 0)
		length++;

/*
	Walk the bit string converting +ve and 0 into 1s (i.e. postings in a postings list)
	and add it to the weighted signature
*/
vector = signature->get_vector();
for (bit = 0; bit < width; bit++)
	if (vector[bit] > 0)		// positive values and get encoded as a 1
		feedback_vector[bit]++;
	else if (vector[bit] == 0)	// zero values are encoded (systematically) randomly as 0 or 1
		if (ANT_rand_xorshift64(&seed) & 1)
			feedback_vector[bit]++;

/*
	Clean up
*/
delete [] term_list;
delete document_indexer;
delete signature;

delete parser;
delete readability;
}

/*
	ANT_RELEVANCE_FEEDBACK_TOPSIG::FEEDBACK()
	-----------------------------------------
*/
double *ANT_relevance_feedback_topsig::feedback(ANT_search_engine_result *result, long documents_to_examine)
{
if (feedback_vector == NULL)
	exit(printf("Must call ANT_relevance_feedback::set_topsig_parameters() before feeding back\n"));

memset(feedback_vector, 0, sizeof(*feedback_vector) * width);

populate(result, documents_to_examine);

return feedback_vector;
}

/*
	ANT_RELEVANCE_FEEDBACK_TOPSIG::FEEDBACK()
	-----------------------------------------
*/
ANT_memory_index_one_node **ANT_relevance_feedback_topsig::feedback(ANT_search_engine_result *result, long documents_to_examine, long terms_wanted, long *terms_found)
{
return NULL;			// this kind of feedback is meaningless for TopSig
}

/*
	ANT_RELEVANCE_FEEDBACK_TOPSIG::FEEDBACK()
	-----------------------------------------
*/
ANT_memory_index_one_node **ANT_relevance_feedback_topsig::feedback(char *document, long terms_wanted, long *terms_found)
{
return NULL;			// this kind of feedback is meaningless for TopSig
}

