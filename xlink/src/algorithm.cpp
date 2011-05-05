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
#include "run_config.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

using namespace QLINK;


algorithm::algorithm(ltw_task *task) : ltw_task_(task)/*, crosslink_(false)*/
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

	token_address_ = NULL;

	translate_anchor_for_linking_ = run_config::instance().get_value("translate_anchor_for_linking").length() > 0 && run_config::instance().get_value("translate_anchor_for_linking") != "false";
	if (run_config::instance().get_value("segmented_chinese").length() > 0 && run_config::instance().get_value("segmented_chinese") != "false")
		segmented_ = TRUE;
	else
		segmented_ = FALSE;
}

int algorithm::init_params()
{
	return init_params(cmdline_options::argc, cmdline_options::argv);
}

int algorithm::init_params(int argc, char *argv[])
{
	if (argc < 3)
		usage(argv[0]);		// and exit

	int index_argv_param = 0;

	for (index_argv_param = 1; *argv[index_argv_param] == '-'; index_argv_param++)
		if (strcmp(argv[index_argv_param], "-lowercase") == 0)
			lowercase_only = TRUE;
//		else if (strcmp(argv[index_argv_param], "-noyears") == 0)
//			print_mode |= MODE_NO_4_DIGIT_NUMBERS;
		else if (strncmp(argv[index_argv_param], "-runname:", 9) == 0)
			runname = strchr(argv[index_argv_param], ':') + 1;
//		else if (strncmp(argv[index_argv_param], "-propernounboost:", 17) == 0)
//			proper_noun_boost = atof(strchr(argv[index_argv_param], ':') + 1);
//		else if (strncmp(argv[index_argv_param], "-targets:", 8) == 0)
//			{
//			targets_per_link = atoi(strchr(argv[index_argv_param], ':') + 1);
//			if (targets_per_link <= 0)
//				usage(argv[0]);
//			}
//		else if (strncmp(argv[index_argv_param], "-anchors:", 8) == 0)
//			{
//			anchors_per_run = atoi(strchr(argv[index_argv_param], ':') + 1);
//			if (anchors_per_run <= 0)
//				usage(argv[0]);
//			}
		else if (strncmp(argv[index_argv_param], "-conf", 5) == 0)
		{
			config_ = new algorithm_config(strchr(argv[index_argv_param], ':') + 1);
		}
		else if (strcmp(argv[index_argv_param], "-stopword") == 0) {
			stopword_no_ = TRUE;
		}
	return index_argv_param;
}


/*
	USAGE()
	-------
*/
void algorithm::usage(char *exename)
{
exit(printf("Usage:%s [-lowercase] [-noyears] [-runname:name] [-anchors:<n>] [-targets:<n>] [-propernounboost:<n.n>] <index> <file_to_link> ...\n", exename));
}

void algorithm::clear_token_address()
{

	if (token_address_) {
		delete [] token_address_;
		token_address_ = NULL;
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
	orphan_docid_ = a_topic->get_id();
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

	clear_token_address();

//	long long *all_links_in_file_length = links_->all_links_length_ptr();
//	*all_links_in_file_length = 0;

	//string_clean(filecopy, lowercase_only);
	filecopy = strdup(text_); //xml2txt::instance().clean_tags(xml_, lowercase_only);
	source_ = filecopy;
	if (lowercase_only)
		string_tolower(filecopy);

	current = term_list = new char *[strlen(filecopy)];		// this is the worst case by far
	if (use_utf8_token_matching_) {
		token_address_ = new char *[strlen(filecopy)];
		create_utf8_token_list(filecopy, term_list, segmented_, token_address_);
	}
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
