/*
	MERGER_PARAM_BLOCK.H
	---------------------
*/
#ifndef MERGER_PARAM_BLOCK_H_
#define MERGER_PARAM_BLOCK_H_

#include "indexer_param_block.h"

/*
	class ANT_MERGER_PARAM_BLOCK
	-----------------------------
*/
class ANT_merger_param_block : public ANT_indexer_param_block
{
private:
	int argc;
	char **argv;

public:
	unsigned long compression_scheme;	// bitstring of which compression schemes to use
	long long reporting_frequency;		// the number of terms to merge before reporting progress
	char *doclist_filename;				// name of file containing the internal docid to external docid translations
	char *index_filename;				// name of index file
	long long static_prune_point;		// maximum length of a postings list measured in document IDs
	long stop_word_removal;				// what kinds of stopwords should be removed from the index (NONE, SINGLETONS, etc.)
	long stop_word_df_frequencies;   // remove wrods based on the document frequencies
	double stop_word_df_threshold;		// if df/N is greater than this and (stop_word_removal & PRUNE_DF_FREQUENTS) != 0 then stop the word

protected:
	void compression(char *schemes);
	void term_removal(char *list);

public:
	ANT_merger_param_block(int argc, char *argv[]);
	virtual ~ANT_merger_param_block() {}
	virtual void usage(void);
	virtual void help(void);
	virtual long parse(void);
} ;

#endif  /* MERGER_PARAM_BLOCK_H_ */
