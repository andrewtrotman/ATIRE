/*
	ANT_PARAMS.H
	------------
*/
#ifndef ANT_PARAMS_H_
#define ANT_PARAMS_H_

#include <stdio.h>

/*
	struct ANT_ANT_PARAMS
	-------------------------
*/
enum { MAP, MAgP, MAgPf, RANKEFF, P_AT_N, SUCCESS_AT_N, MAiP } ;									// metrics
enum { /* NONE = 0, */ INEX = 1, TREC, INEX_EFFICIENCY, INEX_FOCUS, INEX_BEP } ;	// evaluation forum
enum { NONE = 0, QUERY = 1, SUM = 2, SHORT = 4 };						// statistics to print (bitstring)
enum { BM25 = 1, IMPACT = 2, READABLE = 4, LMD = 8, LMJM = 16, BOSE_EINSTEIN = 32, DIVERGENCE = 64, TERM_COUNT = 128, ALL_TERMS = 256};		// ranking function
enum { THRESHOLD = 1, WEIGHTED = 2 }; 									// Use FALSE as 0
enum { INDEX_IN_FILE = 0, INDEX_IN_MEMORY = 1};							// read the index from disk or load at startup
enum { /* NONE = 0, */ ARTICLE = 1, RANGE };										// focused retrieval method

struct ANT_ANT_params
{
	long logo;							// display the ANT banner logo or not
	long stemmer;						// which stemmer to use (or 0 for don't stem)
	long stemmer_similarity;			// 'correct' stemming with the thesaurus?
	double stemmer_similarity_threshold;	// threshold for rejecting stems with the thesaurus (default = 0.0)

	long long sort_top_k;				// accurate rank point in the accumulator sort (in the call to sort_results_list())
	long trim_postings_k;				// trim the postigs lists at no fewer than k
	long metric;						// which metric to use (MAP, MAgP, etc)
	long metric_n;						// so we can so P@10, p@30 and so on using one metric
	char *assessments_filename;			// name of the file containing the assessments for the given queries
	char *queries_filename;				// name of a file containing one query per line (first token of each line is the query ID)
	long output_forum;					// export the list of results in INEX or TREC format
	char *output_filename;				// name of the run file
	char *participant_id;				// participant id (used in the export file)
	char *run_name;						// name of the run (used in the export file)
	long results_list_length;			// length of the results list as exported
	long stats;							// which stats to display
	long ranking_function;				// what ranking function should we use?

	double lmd_u;						// the u value for Language Modelling with Dirichlet smoothing
	double lmjm_l;						// the l (lamda) value for Language Modelling with Jelinek-Mercer smoothing
	double bm25_k1, bm25_b;				// the k1 and b value for BM25

	long segmentation;					// query segmentation need or not for east-asian languages, e.g. Chinese
	long file_or_memory;				// the index be loaded into memory at startup (TRUE / FALSE)

	unsigned short port;				// TCP/IP port for ANT SERVER, or 0 for not a server

	long focussing_algorithm;			// focused retrieval relevance ranking algorithm
	long focus_top_k;					// number of focussing accumulators

	long boolean;						//

	const char *index_filename;
	const char *doclist_filename;
	FILE *output;						// the output of ant, stdout is default value

	long length_of_longest_document;
	void *inchannel, *outchannel;
} ;

#endif  /* ANT_PARAMS_H_ */

