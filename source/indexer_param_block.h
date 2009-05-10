/*
	INDEXER_PARAM_BLOCK.H
	---------------------
*/
#ifndef __INDEXER_PARAM_BLOCK_H__
#define __INDEXER_PARAM_BLOCK_H__

/*
	class ANT_INDEXER_PARAM_BLOCK
	-----------------------------
*/
class ANT_indexer_param_block
{
private:
	int argc;
	char **argv;

public:
	long trec_docnos;					// extract the unique id of the document from the DOCNO XML element (-trec | -docno)
	long recursive;						// search for files to index in this directory and directories below (-r)
	unsigned long compression_scheme;	// bitstring of which compression schemes to use
	long compression_validation;		// decompress all compressed strings and measure the decompression performance

protected:
	void compression(char *schemes);

public:
	ANT_indexer_param_block(int argc, char *argv[]);
	void help(void);
	long parse(void);
} ;

#endif __INDEXER_PARAM_BLOCK_H__
