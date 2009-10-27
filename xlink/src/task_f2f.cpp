/*
 * task_f2f.cpp
 *
 *  Created on: Aug 16, 2009
 *      Author: monfee
 */

#include "task_f2f.h"

using namespace QLINK;

task_f2f::task_f2f(std::string &name, std::string& out_algor_name, std::string& in_algor_name)
	: ltw_task(name, out_algor_name, in_algor_name)
{
}

task_f2f::~task_f2f()
{
	// TODO 
}

void task_f2f::perform()
{
	ltw_task::perform();
}

void task_f2f::print_links()
{

}
