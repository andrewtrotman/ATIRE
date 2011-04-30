/*
 * algorithm_ant_crosslink_this.h
 *
 *  Created on: Apr 30, 2011
 *      Author: monfee
 */

#ifndef ALOGRITHM_ANT_CROSSLINK_THIS_H_
#define ALOGRITHM_ANT_CROSSLINK_THIS_H_

#include "algorithm_ant_link_this.h"

namespace QLINK
{

class algorithm_ant_crosslink_this: public QLINK::algorithm_ant_link_this
{
private:
	algorithm_ant_link_this		find_anchors_with_this_;

protected:
	virtual ANT_link_term *find_term_in_list(const char *value);
	virtual void add_link(ANT_link_term *term, char **term_list);

public:
	algorithm_ant_crosslink_this(ltw_task *task);
	virtual ~algorithm_ant_crosslink_this();

	virtual int init_params(int argc, char *argv[]);
};

}

#endif /* ALOGRITHM_ANT_CROSSLINK_THIS_H_ */
