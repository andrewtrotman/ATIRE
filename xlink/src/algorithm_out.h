/*
 * algorithm_out.h
 *
 *  Created on: Sep 6, 2010
 *      Author: monfee
 */

#ifndef ALGORITHM_OUT_H_
#define ALGORITHM_OUT_H_

#include "algorithm.h"
#include "application_out.h"

namespace QLINK
{

	class algorithm_out: public QLINK::algorithm
	{
	public:
		algorithm_out(ltw_task *task);
//		algorithm_out() {}
		virtual ~algorithm_out();

	public:
		void list_anchors(application_out &anchors_list);

	protected:
		virtual void recommend_anchors(links* lx, char **term_list, const char *source);
		virtual void add_link(ANT_link_term *term, char **term_list, long offset) {}
		long assign_link_term(char *buffer, char **term_list);
	};

}

#endif /* ALGORITHM_OUT_H_ */
