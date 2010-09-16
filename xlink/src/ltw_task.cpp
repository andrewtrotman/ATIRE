/*
 * ltw_task.cpp
 *
 *  Created on: Aug 16, 2009
 *      Author: monfee
 */

#include "ltw_task.h"
#include "algorithm_ant_link_this.h"
#include "algorithm_ant_link_this_and_page_name.h"
#include "algorithm_ltw_tang.h"
#include "algorithm_page_name.h"
#include "algorithm_bep.h"
#include "algorithm_ltw_incoming_search_tn.h"
#include "algorithm_ltw_incoming_a2b_search_tn.h"
#include "algorithm_ltara_base.h"
#include "sys_files.h"
#include "cmdline_options.h"
#include "ltw_topic.h"
#include "incoming_links.h"
#include "outgoing_links.h"
#include "corpus.h"
#include "corpus_txt.h"

#include <iostream>

using namespace std;
using namespace QLINK;

int ltw_task::topic_param_start = 1;

ltw_task::ltw_task(std::string& name, std::string& out_algor_name, std::string& in_algor_name)
	: name_(name), 	algor_out_name_(out_algor_name), algor_in_name_(in_algor_name)
{
	outgoings_ = NULL;
	incomings_ = NULL;

	algor_out_ = NULL;
	algor_in_ = NULL;
	algor_bep_ = NULL;

	init();
}

ltw_task::~ltw_task()
{
	if (outgoings_)
		delete outgoings_;
	if (incomings_)
		delete incomings_;

	if (algor_out_)
		delete algor_out_;

	if (algor_in_)
		delete algor_in_;

	if (algor_bep_)
		delete algor_bep_;
}

void ltw_task::init()
{
	//topic_param_start_ = 1;
	type(IR_TASK_LTW);

//	incomings_ = new links();
//	outgoings_ = new links();

	switch (get_algorithm_outgoing_type()) {
	case LTW_ANT:
		algor_out_ = new algorithm_ant_link_this();
		//topic_param_start = algor_out_->init();
		break;
	case LTW_PAGE_NAME:
		algor_out_ = new algorithm_page_name();
		//topic_param_start = algor_out_->init();
		break;
	case LTW_ANT_AND_PAGE:
		algor_out_ = new algorithm_ant_link_this_and_page_name();
		//topic_param_start = algor_out_->init();
		break;
	case LTW_TANG:
		//algor_out_ = new algorithm_ltw_tang();
		break;
	case LTW_TEARA_BASE:
		algor_out_ = new algorithm_ltara_base();
		break;
	case LTW_NONE:
	default:
		cerr << "No algorithm for generating outgoing links specified" << endl;
		break;
	}

	if (algor_out_)
		algor_out_->init_params();

	switch (get_algorithm_incoming_type()) {
	case LTW_A2B_INCOMING_SEARCH_TN:
		algor_in_ = new algorithm_ltw_incoming_a2b_search_tn();
		break;
	case LTW_INCOMING_SEARCH_TN:
		algor_in_ = new algorithm_ltw_incoming_search_tn();
		break;
	case LTW_TANG:
		algor_in_ = new algorithm();
		break;
	case LTW_NONE:
	default:
		cerr << "No algorithm for gernerating incoming links specified" << endl;
		break;
	}
}

void ltw_task::set_alorithm_bep(string name)
{
	if (name.length() > 0)
		algor_bep_ = new algorithm_bep(name);
}

void ltw_task::perform()
{
	int count = 0;
	long param = topic_param_start;
	for (; param < cmdline_options::argc; param++) {
		sys_files disk;
		disk.pattern("*.[xX][mM][lL]");
		char *filename = cmdline_options::argv[param];
		disk.list(filename);
		if (disk.isdir(filename)) {
			corpus::instance().base(filename);
			corpus_txt::instance().base(filename);
		}
		const char *name = NULL;

		for (name = disk.first(); name != NULL ; name = disk.next())
		{
			count++;
			//cerr << "Processing #" << count << ": " << name << endl;
			//char *file = disk.read_entire_file(name);
			ltw_topic a_topic(name);
			a_topic.xml_to_text();
			a_topic.set_ltw_task(this);
			a_topic.print_header();

			if (algor_out_) {
				if (outgoings_) {
					delete outgoings_;
					outgoings_ = NULL;
				}
				outgoings_ = new outgoing_links(&a_topic);
				outgoings_->set_bep_algorithm(algor_bep_);
				algor_out_->set_links_container(outgoings_);
				algor_out_->process_topic(&a_topic);
				//if (algor_out_name_ != "LTW_ANT")
				//if (is_a2b_task())
					outgoings_->find_anchor();

				// output
				outgoings_->print();
			}
			if (algor_in_) {
				if (incomings_) {
					delete incomings_;
					incomings_ = NULL;
				}
				incomings_ = new incoming_links(&a_topic);
				incomings_->set_bep_algorithm(algor_bep_);
				algor_in_->set_links_container(incomings_);
				algor_in_->process_topic(&a_topic);
				//if (is_a2b_task())
					incomings_->find_anchor();

				// output
				incomings_->print();
			}
			a_topic.print_footer();

			//delete [] file;
		}
	}
//	outgoings_->create();
//	incomings_->create();
}

void ltw_task::print_links()
{
	//if (algor_out_name_ != "LTW_ANT")
//		outgoings_->print();
//
//	incomings_->print();
}
