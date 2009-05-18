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
	enum { MAP, MAgP } ;
	enum { /* NONE = 0, */ INEX = 1, TREC = 2 } ;
	enum { NONE = 0, QUERY = 1, SUM = 2, SHORT = 4 };		// bitstring

private:
	int argc;
	char **argv;

public:
	long logo;					// display the ANT banner logo or not
	long stemmer;				// which stemmer to use (or 0 for don't stem)
	long long sort_top_k;		// accurate rank point in the accumulator sort (in the call to sort_results_list())
	long metric;				// which metric to use (MAP, MAgP, etc)
	char *assessments_filename;	// name of the file containing the assessments for the given queries
	char *queries_filename;		// name of a file containing one query per line (first token of each line is the query ID)
	long output_forum;			// export the list of results in INEX or TREC format
	char *output_filename;		// name of the run file
	char *participant_id;		// participant id (used in the export file)
	char *run_name;				// name of the run (used in the export file)
	long results_list_length;	// length of the results list as exported
	long stats;					// which stats to display

private:
	void export_format(char *forum);
	void term_expansion(char *which);
	void set_metric(char *which);
	void set_stats(char *which);

public:
	ANT_ANT_param_block(int argc, char *argv[]);
	~ANT_ANT_param_block();

	void usage(void);
	void help(void);
	long parse(void);
} ;

#endif __ANT_PARAM_BLOCK_H__

