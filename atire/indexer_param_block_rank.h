/*
	INDEXER_PARAM_BLOCK_RANK.H
	--------------------------
*/
#ifndef INDEXER_PARAM_BLOCK_RANK_H_
#define INDEXER_PARAM_BLOCK_RANK_H_

class ANT_ranking_function;

/*
	class ANT_INDEXER_PARAM_BLOCK_RANK
	----------------------------------
*/
class ANT_indexer_param_block_rank
{
public:
	long long ranking_function;				// which ranking function should we use?
	long long feedback_ranking_function;	// which ranking function should we use for feedback?

	double p1, p2, p3;							// ranking function parameters (e.g. k1 and b)
	double feedback_p1, feedback_p2, feedback_p3;		// ranking function parameters for the feedbacker (e.g. k1 and b)

	char *field_name;								// field to rank on for pregens

private:
	const char *isdefault(long long what) { return ranking_function == what ? "[default]" : ""; }

protected:
	static int get_three_parameters(char *from, double *first, double *second, double *third);
	static int get_two_parameters(char *from, double *first, double *second);
	static int get_one_parameter(char *from, double *into);

	virtual void help(char *title, char switch_char, long long allowable);
public:
	ANT_indexer_param_block_rank();
	virtual ~ANT_indexer_param_block_rank();

	virtual int set_ranker(char *which, long feedbacker = false);
};

#endif /* INDEXER_PARAM_BLOCK_RANK_H_ */
