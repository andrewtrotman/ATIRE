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
	virtual ANT_NEXI_term *make_pool(long pool_size) { return new ANT_NEXI_term_ant[pool_size]; }
} ;

#endif /* NEXI_ANT_H_ */
