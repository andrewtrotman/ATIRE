/*
	NEXI_ANT.H
	----------
*/
#ifndef NEXI_ANT_H_
#define NEXI_ANT_H_

#include "NEXI.h"
#include "NEXI_term_ant.h"

/*
	class ANT_NEXI_ANT
	------------------
*/
class ANT_NEXI_ant : public ANT_NEXI
{
protected:
	ANT_NEXI_term_ant *ant_pool;

protected:
	virtual ANT_NEXI_term_ant *make_pool(long pool_size) { return ant_pool = new ANT_NEXI_term_ant[pool_size]; }
	virtual ANT_NEXI_term *next_free_node() { return ant_pool + pool_used; } 

public:
	ANT_NEXI_ant() : ANT_NEXI(0) { ant_pool = make_pool(MAX_NEXI_TERMS); }
	ANT_NEXI_term_ant *parse(char *expression) { return (ANT_NEXI_term_ant *)ANT_NEXI::parse(expression); }
} ;

#endif /* NEXI_ANT_H_ */
