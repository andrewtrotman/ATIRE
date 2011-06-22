/*
	ATIRE_ENGINE_PARAM_BLOCK.H
	--------------------------
*/
#ifndef ATIRE_ENGINE_PARAM_BLOCK_H_
#define ATIRE_ENGINE_PARAM_BLOCK_H_

/*
	class ATIRE_ENGINE_PARAM_BLOCK
	------------------------------
*/
class ATIRE_engine_param_block
{
public:
	long argc;				// command line parameter count (from main())
	char **argv;			// command line parameters (from main())
	long logo;				// should we display the welcome logo?

protected:
	virtual void usage(void) = 0;
	virtual void help(void) = 0;

public:
	ATIRE_engine_param_block(long argc, char *argv[]) { this->argc = argc; this->argv = argv; }
	virtual ~ATIRE_engine_param_block() {}

	virtual long parse(void) = 0;
} ;

#endif /* ATIRE_ENGINE_PARAM_BLOCK_H_ */
