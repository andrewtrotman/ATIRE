/*
 * algorithm_out.h
 *
 *  Created on: Sep 6, 2010
 *      Author: monfee
 */

#ifndef ALGORITHM_OUT_H_
#define ALGORITHM_OUT_H_

#include "algorithm.h"

namespace QLINK
{

	class algorithm_out: public QLINK::algorithm
	{
	public:
		algorithm_out();
		virtual ~algorithm_out();

	public:
		virtual void recommend_anchors(links* lx, char **term_list, const char *source) = 0;
		void list_anchors();
	};

}

#endif /* ALGORITHM_OUT_H_ */
