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
	protected:
		bool use_utf8_token_matching_; // which means that matching without spaces, all the spaces in string will be removed

	public:
		algorithm_out(ltw_task *task);
		algorithm_out() {}
		virtual ~algorithm_out();

	public:
		virtual void recommend_anchors(links* lx, char **term_list, const char *source) = 0;
		void list_anchors(application_out &anchors_list);
	};

}

#endif /* ALGORITHM_OUT_H_ */
