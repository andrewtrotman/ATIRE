/*
 * algorithm.cpp
 *
 *  Created on: Aug 3, 2009
 *      Author: monfee
 */

#include "algorithm.h"
#include "ltw_topic.h"
#include "ant_link_parts.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

using namespace QLINK;


algorithm::algorithm(links *links_list)
{
	set_links_container(links_list);
	init();
}

algorithm::algorithm()
{
	init();
}

algorithm::~algorithm()
{
	if (config_) {
		delete config_;
		config_ = NULL;
	}
}

void algorithm::init()
{
	config_ = NULL;
	lowercase_only = 0;
}

void algorithm::set_links_container(links *container)
{
	links_ = container;

//	links_in_orphan = links_->links_in_orphan;
//	all_links_in_file = links_->all_links_in_file;
//	all_links_in_file_length = &links_->all_links_in_file_length;
//	links_in_orphan_length = &links_->links_in_orphan_length;
}

void algorithm::process_topic(ltw_topic *a_topic)
{
	text_ = a_topic->get_text();
	process_topic_text();
}

void algorithm::process_links(ltw_topic *a_topic, algorithm *bep_algor)
{
	text_ = a_topic->get_text();
}

void algorithm::process_topic_text()
{

	static const char *seperators = " ";
	char *token;
	char **term_list/*, **first, **last, **current*/;
	char *filecopy = strdup(text_);


//	long long *all_links_in_file_length = links_->all_links_length_ptr();
//	*all_links_in_file_length = 0;

	//string_clean(filecopy, lowercase_only, 0);
	if (lowercase_only)
		string_tolower(filecopy);

	/*current = */term_list = new char *[strlen(filecopy)];		// this is the worst case by far
//	for (token = strtok(filecopy, seperators); token != NULL; token = strtok(NULL, seperators))
//		*current++ = token;
//	*current = NULL;
	string_to_list(filecopy, term_list);

	process_terms(term_list, filecopy);

	// segmnet fault for delete term list
	delete [] term_list;
	free(filecopy);
}
