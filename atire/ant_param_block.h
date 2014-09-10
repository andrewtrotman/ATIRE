/*
	ANT_PARAM_BLOCK.H
	-----------------
*/
#ifndef ANT_PARAM_BLOCK_H_
#define ANT_PARAM_BLOCK_H_

#include "fundamental_types.h"
#include "indexer_param_block_rank.h"
#include "indexer_param_block_stem.h"
#include "indexer_param_block_topsig.h"
#include "evaluator.h"

#define MAX_PREGEN_COUNT 128

/*
	class ANT_ANT_PARAM_BLOCK
	-------------------------
*/
class ANT_ANT_param_block : public ANT_indexer_param_block_rank, public ANT_indexer_param_block_stem, public ANT_indexer_param_block_topsig
{
public:
	enum { NONE = 0, QUERY = 1, SUM = 2, SHORT = 4, PRECISION = 8 };					// statistics to print (bitstring)
	enum { /* NONE = 0, */ INEX = 1, TREC, INEX_EFFICIENCY, INEX_FOCUS, INEX_BEP }; 	// evaluation forum
	enum { INDEX_IN_FILE, INDEX_IN_MEMORY};												// read the index from disk or load at startup
	enum { /* NONE = 0, */ ARTICLE = 1, RANGE };										// focused retrieval method
	enum { TERM_AT_A_TIME, QUANTUM_AT_A_TIME };											// processing strategy (term or quantum at a time)
	enum { QUANTUM_STOP_NONE = 0,             // no early termination
			 QUANTUM_STOP_DIFF = 1,             // early terminated based on the difference of the top k and k+1
			 QUANTUM_STOP_DIFF_SMALLEST = 2,    // early terminated based on the smallest difference among the top k documents
			 QUANTUM_STOP_DIFF_LARGEST = 4      // early terminated based on the difference between the largest and second largest in the top documents
			};
	enum { /*NONE = 0, */ STOPWORDS_PUURULA = 1,  STOPWORDS_NCBI = 2, STOPWORDS_SHORT = 4, STOPWORDS_NUMBERS = 8, STOPWORDS_ATIRE = 16 };

public:
	int argc;
	char **argv;

	long logo;								// display the ANT banner logo or not
	long long sort_top_k;				// accurate rank point in the accumulator sort (in the call to sort_results_list())
	long trim_postings_k;				// trim the postigs lists at no fewer than k
	ANT_evaluator *evaluator;			// a proxy for all the evaluation functions we want
	char *assessments_filename;			// name of the file containing the assessments for the given queries
	char *queries_filename;				// name of a file containing one query per line (first token of each line is the query ID)
	long output_forum;					// export the list of results in INEX or TREC format
	char *output_filename;				// name of the run file
	char *participant_id;				// participant id (used in the export file)
	char *run_name;						// name of the run (used in the export file)
	long results_list_length;			// length of the results list as exported
	long stats;								// which stats to display

	/*
		Because these are expected to be replaced several times during the lifetime of this class,
		this class owns the memory for these strings and calls delete [] on them:
	*/
	char *index_filename;				// Filename of index to read
	char *doclist_filename;				// Filename of doclist to read

	char *pregen_names[MAX_PREGEN_COUNT];// pregens to load
	int pregen_count;					// count of entries in list
	double pregen_ratio;						// the ratio of score to be used in the rsv values

	long segmentation;					// query segmentation need or not for east-asian languages, e.g. Chinese
	long file_or_memory;				// the index be loaded into memory at startup (TRUE / FALSE)

	unsigned short port;				// TCP/IP port for ANT SERVER, or 0 for not a server

	long focussing_algorithm;			// focused retrieval relevance ranking algorithm
	long focus_top_k;					// number of focussing accumulators

	long snippet_algorithm;				// how to generate snippets
	char *snippet_tag;					// if they require an XML tag then use this one
	long snippet_length;				// snippets should be no longer than this (in characters)
	long snippet_stemmer;				// algorithm to use to stem keywords and search terms when looking for snippets
	long snippet_word_cloud_terms;		// maximum number of terms to generate when generating word coulds

	long title_algorithm;				// should we construct a title or not (this is a snippet algorithm)
	char *title_tag;					// use the contents of this element as the title of the document
	long title_length;					// titles should be no longer than this (in characters)

	long query_type;					// NEXI, Boolean, and optionally additionally with relevance feedback
	char *query_fields;					// in the case of a TREC topic file, which fields should we use (i.e. title, etc).
	long query_stopping;				// type of stop word removal to apply.
	long feedbacker;					// relevance feedback algorithm to use
	long feedback_documents;			// documents to analyse for feedback terms
	long feedback_terms;				// terms to add to the query in relevance feedback
	double feedback_lambda;				// used for linear interpolation of initial and feedback result sets

	long accumulator_sort;				// the method to be used to sort accumulators

	unsigned long expander_tf_types;	// types of synet expansions for tf-expansion
	unsigned long expander_query_types;	// types of synet expansions for query expansion

	long processing_strategy;			// term-at-a-time or quantum-at-a-time
	uint8_t quantum_stopping;           // the early termination strategy for quantum-at-a-time
	long quantization_bits;				// how many bits to quantize into
	long quantization;					// whether or not we should quantize

private:
	void export_format(char *forum);
	void set_stats(char *which);
	void set_focused_ranker(char *which);
	void set_feedbacker(char *which);
	unsigned long decode_expansion_types(char *which);

public:
	ANT_ANT_param_block(int argc, char *argv[]);
	virtual ~ANT_ANT_param_block();

	virtual void set_index_filename(char * filename);
	virtual void set_doclist_filename(char * filename);
	virtual char* swap_index_filename(char * filename);
	virtual char* swap_doclist_filename(char * filename);

	virtual void usage(void);
	using ANT_indexer_param_block_stem::help;
	virtual void help(void);
	using ANT_indexer_param_block_rank::help;
	virtual long parse(void);
} ;

#endif  /* ANT_PARAM_BLOCK_H_ */

