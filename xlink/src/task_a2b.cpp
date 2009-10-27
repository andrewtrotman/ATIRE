/*
 * task_a2b.cpp
 *
 *  Created on: Aug 16, 2009
 *      Author: monfee
 */

#include "task_a2b.h"

using namespace QLINK;

task_a2b::task_a2b(std::string &name, std::string& out_algor_name, std::string& in_algor_name)
	: ltw_task(name, out_algor_name, in_algor_name)
{
}

task_a2b::~task_a2b()
{
	// TODO 
}

void task_a2b::perform()
{
	ltw_task::perform();
}

void task_a2b::print_links()
{

}
