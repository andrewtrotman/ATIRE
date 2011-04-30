/*
 * algorithm_ltw_tang.cpp
 *
 *  Created on: Aug 13, 2009
 *      Author: monfee
 */

#include "algorithm_ltw_tang.h"
#include <iostream>

using namespace std;
using namespace QLINK;


algorithm_ltw_tang::algorithm_ltw_tang(ltw_task *task) : algorithm_out(task)
{

}

algorithm_ltw_tang::~algorithm_ltw_tang()
{
}

int algorithm_ltw_tang::init()
{
	cerr << "creating run using LTW_TANG algorithm..." << endl;
}
