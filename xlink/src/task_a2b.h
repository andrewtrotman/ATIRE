/*
 * task_a2b.h
 *
 *  Created on: Aug 16, 2009
 *      Author: monfee
 */

#ifndef TASK_A2B_H_
#define TASK_A2B_H_

#include "ltw_task.h"

namespace QLINK {

class task_a2b: public ltw_task
{
public:
	task_a2b(std::string &name, std::string& out_algor_name, std::string& in_algor_name);
	virtual ~task_a2b();

	void perform();
	void print_links();
};

}

#endif /* TASK_A2B_H_ */
