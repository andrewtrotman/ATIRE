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
	char *connect_string;			// address:port of server we're connecting to
	char *queries_filename;			// name of the file that has the list of queries in it
	long long results_list_length;	// number of results to return to the client

protected:
	virtual void usage(void);
	virtual void help(void);

public:
	ATIRE_client_param_block(long argc, char *argv[]);
	virtual ~ATIRE_client_param_block() { }

	long parse(void);
} ;

#endif /* ATIRE_CLIENT_PARAM_BLOCK_H_ */
