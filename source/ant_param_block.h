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
private:
	int argc;
	char **argv;

public:
	long logo;					// display the ANT banner logo or not
	long stemmer;				// which stemmer to use (or 0 for don't stem)
	long long sort_top_k;		// accurate rank point in the accumulator sort (in the call to sort_results_list())

private:
	void term_expansion(char *which);

public:
	ANT_ANT_param_block(int argc, char *argv[]);
	void usage(void);
	void help(void);
	long parse(void);
} ;

#endif __ANT_PARAM_BLOCK_H__

