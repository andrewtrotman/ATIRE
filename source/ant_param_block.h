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

private:
	int argc;
	char **argv;

public:
	long logo;					// display the ANT banner logo or not
	long stemmer;				// which stemmer to use (or 0 for don't stem)
	long long sort_top_k;		// accurate rank point in the accumulator sort (in the call to sort_results_list())
	long metric;				// which metric to use (MAP, MAgP, etc)
	char *assessments_filename;	// name of the file containing the assessments for the given queries

private:
	void term_expansion(char *which);
	void assessments(char *filename);
	void set_metric(char *which);

public:
	ANT_ANT_param_block(int argc, char *argv[]);
	~ANT_ANT_param_block();

	void usage(void);
	void help(void);
	long parse(void);
} ;

#endif __ANT_PARAM_BLOCK_H__

