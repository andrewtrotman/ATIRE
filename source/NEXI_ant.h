/*
	NEXI_ANT.H
	----------
*/
#ifndef NEXI_ANT_H_
#define NEXI_ANT_H_

#include "NEXI.h"
#include "NEXI_ant.h"
#include "NEXI_term_ant.h"

/*
	class ANT_NEXI_ANT
	------------------
*/
class ANT_NEXI_ant : public ANT_NEXI
{
protected:
	ANT_NEXI_term_ant *ant_pool;
	double tf_weight;					// the tf value used in the ranking function is (tf * tf_weight)
	double rsv_weight;					// the value added to the accumulators is rsv_weight * rank(tf)

protected:
	virtual ANT_NEXI_term *next_free_node() { return ant_pool + pool_used; } 

public:
	ANT_NEXI_ant() : ANT_NEXI(0) { ant_pool = new ANT_NEXI_term_ant[MAX_NEXI_TERMS]; tf_weight = rsv_weight = 1; }
	virtual ~ANT_NEXI_ant() { delete [] ant_pool; }

	ANT_NEXI_term_ant *parse(char *expression) { return (ANT_NEXI_term_ant *)ANT_NEXI::parse(expression); }
	virtual ANT_query *parse(ANT_query *into, char *expression);
} ;

#endif /* NEXI_ANT_H_ */
