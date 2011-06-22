/*
	ATIRE_BROKER_PARAM_BLOCK.H
	--------------------------
*/
#ifndef ATIRE_BROKER_PARAM_BLOCK_H_
#define ATIRE_BROKER_PARAM_BLOCK_H_

#include "atire_engine_param_block.h"

/*
	class ATIRE_BROKER_PARAM_BLOCK
	------------------------------
*/
class ATIRE_broker_param_block : public ATIRE_engine_param_block
{
public:
	unsigned short port;			// the port number that the broker is listening on (or zero for stdin)
	char *queries_filename;			// name of the file that has the list of queries in it
	char **servers;					// NULL terminated list of server:port addresses of servers
	long long number_of_servers;	// the length of the servers array
	long long results_list_length;	// number of results to return to the client

protected:
	virtual void usage(void);
	virtual void help(void);

public:
	ATIRE_broker_param_block(long argc, char *argv[]);
	virtual ~ATIRE_broker_param_block() { delete [] servers; }

	long parse(void);
} ;

#endif /* ATIRE_BROKER_PARAM_BLOCK_H_ */
