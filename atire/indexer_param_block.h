/*
	INDEXER_PARAM_BLOCK.H
	---------------------
*/
#ifndef INDEXER_PARAM_BLOCK_H_
#define INDEXER_PARAM_BLOCK_H_

#include "indexer_param_block_rank.h"
#include "indexer_param_block_stem.h"
#include "indexer_param_block_pregen.h"
#include "indexer_param_block_topsig.h"
#include "pregen.h"

/*
	class ANT_INDEXER_PARAM_BLOCK
	-----------------------------
*/
class ANT_indexer_param_block : public ANT_indexer_param_block_rank, public ANT_indexer_param_block_stem, public ANT_indexer_param_block_pregen, public ANT_indexer_param_block_topsig
{
public:
	enum { STAT_MEMORY = 1, STAT_TIME = 2, STAT_COMPRESSION = 4, STAT_SUMMARY = 8 } ;
	enum { NONE = 0, DIRECTORIES, TAR, TAR_BZ2, TAR_GZ, TAR_LZO, PKZIP, WARC_GZ, RECURSIVE_WARC_GZ, CSV, TSV, TREC, RECURSIVE_TREC, TRECBIG, VBULLETIN, PHPBB, MYSQL };

private:
	int argc;
	char **argv;

public:
	long recursive;						// search for files to index in this directory and directories below or in tar files (-r options)
	unsigned long compression_scheme;	// bitstring of which compression schemes to use
	long compression_validation;		// decompress all compressed strings and measure the decompression performance
	long statistics;					// bit pattern of which stats to print at the end of indexing
	long logo;							// display (or suppress) the banner on startup
	long long reporting_frequency;		// the number of documents to index before reporting the memory usage stats
	long segmentation;					// need segmentation or not for east-asian languages, e.g. Chinese
	unsigned long readability_measure; 	// readability measure to calculate
	long document_compression_scheme;	// should we and how should we store the documents in the repository?
	char *doclist_filename;				// name of file containing the internal docid to external docid translations
	char *index_filename;				// name of index file
	char *spam_filename;				// where to load spam files from
	long long spam_threshold; 	// spam percentile threshold
	long long mime_filter;
	long long static_prune_point;		// maximum length of a postings list measured in document IDs
	long stop_word_removal;				// what kinds of stopwords should be removed from the index (NONE, SINGLETONS, etc.)
	long stop_word_df_frequencies;   // remove wrods based on the document frequencies
	double stop_word_df_threshold;		// if df/N is greater than this and (stop_word_removal & PRUNE_DF_FREQUENTS) != 0 then stop the word
	long scrubbing;					// once we've read the source file from disk, scan from start to finish removing optionally '\0' characters and non-ascii characters
	long filter_method;
	char *filter_filename; // file containing documents to filter out

protected:
	void document_compression(char *scheme);
	void compression(char *schemes);
	void readability(char *measures);
	void stats(char *stat_list);
	void segment(char *segment_flag);
	void term_removal(char *list);
	void scrub(char *scrub_what);

public:
	ANT_indexer_param_block(int argc, char *argv[]);
	virtual ~ANT_indexer_param_block() {}
	virtual void usage(void);
	using ANT_indexer_param_block_stem::help;
	using ANT_indexer_param_block_rank::help;
	virtual void help(void);
	virtual long parse(void);
} ;

#endif  /* INDEXER_PARAM_BLOCK_H_ */
