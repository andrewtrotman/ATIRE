/*
	NEXI_TERM_ANT.H
	---------------
*/
#ifndef NEXI_TERM_ANT_H_
#define NEXI_TERM_ANT_H_

#include "NEXI_term.h"
#include "search_engine_btree_leaf.h"

/*
	class ANT_NEXI_TERM_ANT
	-----------------------
*/
class ANT_NEXI_term_ant : public ANT_NEXI_term
{
public:
	ANT_search_engine_btree_leaf term_details;
} ;

#endif /* NEXI_TERM_ANT_H_ */

