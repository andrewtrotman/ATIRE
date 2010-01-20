/*
 * algorithm_ltw_incoming_search_tn.cpp
 *
 *  Created on: Aug 17, 2009
 *      Author: monfee
 */

#include "algorithm_ltw_incoming_search_tn.h"
#include "ant_link_parts.h"
#include "ltw_topic.h"

#include <stdio.h>

using namespace QLINK;


algorithm_ltw_incoming_search_tn::algorithm_ltw_incoming_search_tn()
{
	init_search_engine();
}

algorithm_ltw_incoming_search_tn::~algorithm_ltw_incoming_search_tn()
{
	ant_free(ant_);
}

void algorithm_ltw_incoming_search_tn::init_search_engine()
{
	docids_ = NULL;

	ant_ = ant_easy_init();
	ant_params(ant_)->logo = FALSE;
	ant_params(ant_)->stats = NONE;
	ant_params(ant_)->output = stderr;

	ant_setup(ant_);
}

void algorithm_ltw_incoming_search_tn::process_topic(ltw_topic *a_topic)
{
	hits_ = 0;
	long long result = 0;
	docids_ = ant_search(ant_, &hits_, (char *)a_topic->get_name());
	ANT_link_term *node = NULL;

	const long long last_to_list = links_->links_to_print();
	long long size = last_to_list * 2;
	if (size > hits_)
		size = hits_;
	long doc_id = -1;
	long count = 0;
	for (; result < last_to_list && count < size; result++) {
		//printf("%lld:%s\n", result + 1, docids_[result]);
		doc_id = result_to_id(docids_[result]);

		if (doc_id == a_topic->get_id())
			continue;

		//node = new ANT_link_term;
		//node->term = strdup(a_topic->get_name());
		links_->push_link((char *)a_topic->get_name(), doc_id, NULL);
		count++;
	}
}
