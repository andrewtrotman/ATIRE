/*
	ANT_PARAM_BLOCK.H
	-----------------
*/
#ifndef ANT_PARAM_BLOCK_H_
#define ANT_PARAM_BLOCK_H_

#include "indexer_param_block_rank.h"

/*
	class ANT_ANT_PARAM_BLOCK
	-------------------------
*/
class ANT_ANT_param_block : public ANT_indexer_param_block_rank
{
public:
	enum { MAP, MAgP, MAgPf, RANKEFF, P_AT_N, SUCCESS_AT_N, MAiP } ;					// metrics
	enum { NONE = 0, QUERY = 1, SUM = 2, SHORT = 4 };									// statistics to print (bitstring)
	enum { /* NONE = 0, */ INEX = 1, TREC = 2, INEX_EFFICIENCY = 4, INEX_FOCUS = 8, INEX_BEP = 16 } ;	// evaluation forum
	enum { /* NONE = 0, */ THRESHOLD = 1, WEIGHTED = 2 }; 								// stemming stuff
	enum { INDEX_IN_FILE = 0, INDEX_IN_MEMORY = 1};										// read the index from disk or load at startup
	enum { /* NONE = 0, */ RANGE = 1};													// focused retrieval

private:
	int argc;
	char **argv;

public:
	long logo;							// display the ANT banner logo or not
	long stemmer;						// which stemmer to use (or 0 for don't stem)
	long stemmer_similarity;			// 'correct' stemming with the thesaurus?
	double stemmer_similarity_threshold;	// threshold for rejecting stems with the thesaurus (default = 0.0)

	long long sort_top_k;				// accurate rank point in the accumulator sort (in the call to sort_results_list())
	long long trim_postings_k;			// trim the postigs lists at no fewer than k
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

	long segmentation;					// query segmentation need or not for east-asian languages, e.g. Chinese
	long file_or_memory;				// the index be loaded into memory at startup (TRUE / FALSE)

	unsigned short port;				// TCP/IP port for ANT SERVER, or 0 for not a server

	long focussing_algorithm;			// focused retrieval relevance ranking algorithm
	long focus_top_k;					// number of focussing accumulators

private:
	void export_format(char *forum);
	void term_expansion(char *which);
	void set_metric(char *which);
	void set_stats(char *which);
	void set_focused_ranker(char *which);

public:
	ANT_ANT_param_block(int argc, char *argv[]);
	virtual ~ANT_ANT_param_block();

	virtual void usage(void);
	virtual void help(void);
	virtual long parse(void);
} ;

#endif  /* ANT_PARAM_BLOCK_H_ */

