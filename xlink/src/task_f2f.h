/*
 * task_f2f.h
 *
 *  Created on: Aug 16, 2009
 *      Author: monfee
 */

#ifndef TASK_F2F_H_
#define TASK_F2F_H_

#include "ltw_task.h"

namespace QLINK {

	class task_f2f: public ltw_task
	{
	public:
		task_f2f(std::string &name, std::string& out_algor_name, std::string& in_algor_name);
		virtual ~task_f2f();

		void perform();
		void print_links();
	};

}

#endif /* TASK_F2F_H_ */
