/*
	ATIRE_CLIENT_PARAM_BLOCK.H
	--------------------------
*/
#ifndef ATIRE_CLIENT_PARAM_BLOCK_H_
#define ATIRE_CLIENT_PARAM_BLOCK_H_

#include "atire_engine_param_block.h"

/*
	class ATIRE_CLIENT_PARAM_BLOCK
	------------------------------
*/
class ATIRE_client_param_block : public ATIRE_engine_param_block
{
public:
	enum { NONE, TREC, INEX_SNIPPET } ;			// evaluation forum
	enum { QUERIES_ANT, QUERIES_TREC};			// is the input file in TREC or ANT format?

public:
	char *connect_string;			// address:port of server we're connecting to
	char *queries_filename;			// name of the file that has the list of queries in it
	long queries_format;			// TREC or ANT query file format?
	char *queries_fields;			// which fields from the TREC (or otherwise) query file should be used
	long long results_list_length;	// number of results to return to the client

	long long output_forum;			// NONE, TREC, or INEX_SNIPPET
	char *output_filename;			// name of the results (run) file
	char *run_name;					// name of the run
	char *group_name;				// name of the INEX group name
	char *run_description;			// INEX run description

protected:
	virtual void usage(void);
	virtual void help(void);

	virtual void export_format(char *forum);
	virtual void export_format_defaults(char *forum);
	virtual void query_format(char *command);

public:
	ATIRE_client_param_block(long argc, char *argv[]);
	virtual ~ATIRE_client_param_block() { }

	long parse(void);
} ;

#endif /* ATIRE_CLIENT_PARAM_BLOCK_H_ */
