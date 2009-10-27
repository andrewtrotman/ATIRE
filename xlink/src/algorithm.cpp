/*
 * algorithm.cpp
 *
 *  Created on: Aug 3, 2009
 *      Author: monfee
 */

#include "algorithm.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

using namespace QLINK;


algorithm::algorithm(links *links_list)
{
	set_links_container(links_list);
}

algorithm::algorithm()
{
}

algorithm::~algorithm()
{
}

void algorithm::set_links_container(links *container)
{
	links_ = container;

//	links_in_orphan = links_->links_in_orphan;
//	all_links_in_file = links_->all_links_in_file;
//	all_links_in_file_length = &links_->all_links_in_file_length;
//	links_in_orphan_length = &links_->links_in_orphan_length;
}
