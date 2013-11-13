/*
	INDEXER_PARAM_BLOCK_STEM.H
	--------------------------
*/
#ifndef INDEXER_PARAM_BLOCK_STEM_H_
#define INDEXER_PARAM_BLOCK_STEM_H_

/*
	class ANT_INDEXER_PARAM_BLOCK_STEM
	----------------------------------
*/
class ANT_indexer_param_block_stem
{
public:
	enum { /* NONE = 0, */ THRESHOLD = 1, WEIGHTED };

public:
	long stemmer;							// which stemmer to use (or 0 for don't stem)
	long stemmer_similarity;				// 'correct' stemming with the thesaurus?
	double stemmer_similarity_threshold;	// threshold for rejecting stems with the thesaurus (default = 0.0)

public:
	ANT_indexer_param_block_stem();
	virtual ~ANT_indexer_param_block_stem() {}

	virtual void help(long has_cutoff);

	void term_expansion(char *which, long has_stem);
} ;

#endif /* INDEXER_PARAM_BLOCK_STEM_H_ */
