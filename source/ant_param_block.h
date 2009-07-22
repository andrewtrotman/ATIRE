/*
	ANT_PARAM_BLOCK.H
	-----------------
*/
#ifndef __ANT_PARAM_BLOCK_H__
#define __ANT_PARAM_BLOCK_H__
/*
	class ANT_ANT_PARAM_BLOCK
	-------------------------
*/
class ANT_ANT_param_block
{
public:
	enum { MAP, MAgP, RANKEFF } ;						// metrics
	enum { /* NONE = 0, */ INEX = 1, TREC = 2 } ;		// evaluation forum
	enum { NONE = 0, QUERY = 1, SUM = 2, SHORT = 4 };	// statistics to print (bitstring)
	enum { BM25, IMPACT, READABLE, LMD, LMJM};			// ranking function

private:
	int argc;
	char **argv;

public:
	long logo;							// display the ANT banner logo or not
	long stemmer;						// which stemmer to use (or 0 for don't stem)
	long long sort_top_k;				// accurate rank point in the accumulator sort (in the call to sort_results_list())
	long long trim_postings_k;			// trim the postigs lists at no fewer than k
	long metric;						// which metric to use (MAP, MAgP, etc)
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

	long segmentation;					// query segmentation need or not for east-asian languages, e.g. Chinese
	long thesaurus;						// 'correct' stemming with the thesaurus?
	double thesaurus_threshold;			// threshold for rejecting stems with the thesaurus (default = 0.0)

private:
	void export_format(char *forum);
	void term_expansion(char *which);
	void set_metric(char *which);
	void set_stats(char *which);
	void set_ranker(char *which);
	void get_one_param(char *from, double *into);

public:
	ANT_ANT_param_block(int argc, char *argv[]);
	~ANT_ANT_param_block();

	void usage(void);
	void help(void);
	long parse(void);
} ;

#endif __ANT_PARAM_BLOCK_H__

