/*
	INDEXER_PARAM_BLOCK_TOPSIG.H
	----------------------------
*/
#ifndef INDEXER_PARAM_BLOCK_TOPSIG_H_
#define INDEXER_PARAM_BLOCK_TOPSIG_H_

/*
	class ANT_INDEXER_PARAM_BLOCK_TOPSIG
	------------------------------------
*/
class ANT_indexer_param_block_topsig
{
public:
	enum { INVERTED_FILE, TOPSIG } ;	// inverted file or TopSig file

public:
	long inversion_type;				// is the index a "standard" inverted file or a TopSig file?
	long topsig_width;					// with of the TopSig signature
	double topsig_density;				// density of the set bits in the TopSig index
	char *topsig_global_stats;			// file containing the cf parameters (from ANT_dictionary)

protected:
	void help(void);
	void topsig(char *params);

public:
	ANT_indexer_param_block_topsig();
	~ANT_indexer_param_block_topsig() {}
} ;


#endif /* INDEXER_PARAM_BLOCK_TOPSIG_H_ */
