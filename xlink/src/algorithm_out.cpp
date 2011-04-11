/*
 * algorithm_out.cpp
 *
 *  Created on: Sep 6, 2010
 *      Author: monfee
 */

#include "algorithm_out.h"
#include "ant_link_term.h"

#include "ltw_task.h"

namespace QLINK
{

algorithm_out::algorithm_out(ltw_task *task) : algorithm(task)
{
	if (ltw_task_->get_source_lang() == "en")
		use_utf8_token_matching_ = false;
	else
		use_utf8_token_matching_ = true;
}

algorithm_out::~algorithm_out()
{

}

void algorithm_out::list_anchors(application_out &anchors_list)
{

	std::vector<link *> link_vector = links_->all_links();
	for (int i = 0; i < link_vector.size(); ++i) {
		anchors_list << link_vector[i]->link_term->term;
		anchors_list << "\n";
	}
}

}
