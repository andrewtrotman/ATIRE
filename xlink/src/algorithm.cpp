/*
 * algorithm.cpp
 *
 *  Created on: Aug 3, 2009
 *      Author: monfee
 */

#include "algorithm.h"
#include "ltw_topic.h"
#include "ant_link_parts.h"
#include "algorithm_config.h"
#include "cmdline_options.h"
#include "xml2txt.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

using namespace QLINK;


algorithm::algorithm(ltw_task *task) : ltw_task_(task)
{
//	set_links_container(links_list);

	init();
}

//algorithm::algorithm()
//{
//	init();
//}

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
	lowercase_only = FALSE; // true by default
	stopword_no_ = FALSE;

	if (ltw_task_->get_source_lang() == "en")
		use_utf8_token_matching_ = false;
	else
		use_utf8_token_matching_ = true;
}

int algorithm::init_params()
{
	return init_params(cmdline_options::argc, cmdline_options::argv);
}

int algorithm::init_params(int argc, char *argv[])
{
	int index_argv_param = 0;

	for (index_argv_param = 1; *argv[index_argv_param] == '-'; index_argv_param++)
		if (strcmp(argv[index_argv_param], "-lowercase") == 0)
			lowercase_only = TRUE;
		else if (strcmp(argv[index_argv_param], "-stopword") == 0) {
			stopword_no_ = TRUE;
			break;
		}

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
	xml_ = a_topic->get_content();
	process_topic_text();
}

void algorithm::process_links(ltw_topic *a_topic, algorithm *bep_algor)
{
	text_ = a_topic->get_text();
	xml_ = a_topic->get_content();
}

void algorithm::process_topic_text()
{

	static const char *seperators = " \n\t\r";
	char *token;
	char **term_list, **current/*, **first, **last*/;
	char *filecopy = NULL; //strdup(xml_);

//	long long *all_links_in_file_length = links_->all_links_length_ptr();
//	*all_links_in_file_length = 0;

	//string_clean(filecopy, lowercase_only);
	filecopy = strdup(text_); //xml2txt::instance().clean_tags(xml_, lowercase_only);
	source_ = filecopy;
//	if (lowercase_only)
//		string_tolower(filecopy);

	current = term_list = new char *[strlen(filecopy)];		// this is the worst case by far
	if (use_utf8_token_matching_)
		create_utf8_token_list(filecopy, term_list);
	else
		{
		for (token = strtok(filecopy, seperators); token != NULL; token = strtok(NULL, seperators))
			*current++ = token;
		*current = NULL;
		}
	//string_to_list(filecopy, term_list);

	process_terms(term_list, filecopy);

	// segmnet fault for delete term list
	if (use_utf8_token_matching_)
		free_utf8_token_list(term_list);
	delete [] term_list;
//	delete [] filecopy;
	free(filecopy);
	//free(filecopy);
}
