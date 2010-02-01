/*
 * algorithm_ant_link_this_and_page_name.cpp
 *
 *  Created on: Oct 6, 2009
 *      Author: monfee
 */

#include "algorithm_ant_link_this_and_page_name.h"
#include "algorithm_config.h"
#include "ant_link_parts.h"

using namespace QLINK;
using namespace std;

algorithm_ant_link_this_and_page_name::algorithm_ant_link_this_and_page_name(links *links_list) :
	algorithm_ant_link_this(links_list)
{
	init();
}

algorithm_ant_link_this_and_page_name::algorithm_ant_link_this_and_page_name()
{
	init();
}

algorithm_ant_link_this_and_page_name::~algorithm_ant_link_this_and_page_name()
{

}

void algorithm_ant_link_this_and_page_name::init()
{
	//config_ = new algorithm_config("algorithm_ant_link_this_and_page_name.conf");
	g_links_ = NULL;
	k_links_ = NULL;
}

void algorithm_ant_link_this_and_page_name::merge_links()
{
	//links_->set_link_managed();
	std::vector<link *>& k_link_array = k_links_->all_links();
	std::vector<link *>& g_link_array = g_links_->all_links();
	link *k_link = NULL, *g_link = NULL;
	int i = 0;
	for (i = 0; i < k_link_array.size(); i++)
		links_->push_link(k_link_array[i]);

	for (i = 0; i < g_link_array.size(); i++) {
		g_link = g_link_array[i];

		k_link = links_->find(g_link->term);
		if (k_link != NULL)
			k_link->gamma += g_link->gamma;
		else {
			link *new_link = links_->push_link(g_link);
			//new_link->require_cleanup();
		}
	}

	links_->sort_links();
}

void algorithm_ant_link_this_and_page_name::process_terms(char **term_list, const char *source)
{
	//algorithm_ant_link_this::process_terms(term_list, source);
	if (k_links_)
		delete k_links_;
	if (g_links_)
		delete g_links_;

	k_links_ = new links;
	g_links_ = new links;

	search_anchor_from_link_analysis(k_links_, term_list, source);
	k_links_->sort_links();

	algorithm_page_name::process_terms(g_links_, term_list, source);

	if (k_links_->all_links_length() > 0)
		calculate_g_links_score(k_links_->all_links()[0]->gamma, k_links_->all_links()[k_links_->all_links_length() - 1]->gamma);
	else
		calculate_g_links_score(1, 0);
	merge_links();

	if (k_links_) {
		delete k_links_;
		k_links_ = NULL;
	}

	if (g_links_) {
		delete g_links_;
		g_links_ = NULL;
	}
}

void algorithm_ant_link_this_and_page_name::calculate_g_links_score(double max, double min)
{
	std::vector<link *>& link_array = g_links_->all_links();
	link *lnk = NULL;

	if (link_array.size() > 0) {
		long max_term_count = count_char(link_array[0]->term, ' ');
		double step = (max - min) / ((double)max_term_count - 1.0);
		for (int i = 0; i < link_array.size(); i++) {
			lnk = link_array[i];
			long term_count = count_char(lnk->term, ' ');
			lnk->gamma = max - (step * (double)(max_term_count - term_count));
		}
	}
}
