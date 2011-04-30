/*
 * algorithm_ltw_tang.h
 *
 *  Created on: Aug 13, 2009
 *      Author: monfee
 */

#ifndef ALGORITHM_LTW_TANG_H_
#define ALGORITHM_LTW_TANG_H_

#include "algorithm_out.h"

namespace QLINK {

	class algorithm_ltw_tang: public algorithm_out
	{
	public:
		algorithm_ltw_tang(ltw_task *task);
		virtual ~algorithm_ltw_tang();

		int init();
	};

}
#endif /* ALGORITHM_LTW_TANG_H_ */
