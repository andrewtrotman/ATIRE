/*
 * algorithm_ltw_incoming_a2b_search_tn.cpp
 *
 *  Created on: Sep 14, 2009
 *      Author: monfee
 */

#include "algorithm_ltw_incoming_a2b_search_tn.h"
#include "link.h"
#include "xml2txt.h"
#include "sys_file.h"
#include "corpus.h"
#include "corpus_txt.h"
#include "ant_link_parts.h"
#include "algorithm_bep.h"
#include "ltw_topic.h"
#include "ant_link_term.h"
#include "ant_link_posting.h"

using namespace QLINK;

algorithm_ltw_incoming_a2b_search_tn::algorithm_ltw_incoming_a2b_search_tn()
{
	//bep_algor_ = new algorithm_bep();
}

algorithm_ltw_incoming_a2b_search_tn::~algorithm_ltw_incoming_a2b_search_tn()
{
	//delete bep_algor_;
}

//void algorithm_ltw_incoming_a2b_search_tn::process_topic(ltw_topic *a_topic)
//{
//	algorithm_ltw_incoming_search_tn::process_topic(a_topic);
//
//	//process_links(a_topic);
//}

//void algorithm_ltw_incoming_a2b_search_tn::process_links(ltw_topic *a_topic)
//{
//	links_->require_cleanup();
//	for (int i = 0; i < links_->all_links_length(); i++)
//		find_anchor_and_bep(a_topic, links_->all_links()[i]);
//}
//
//void algorithm_ltw_incoming_a2b_search_tn::find_anchor_and_bep(ltw_topic *a_topic, link* curr_link)
//{
//	std::string doc = corpus::instance().id2docpath(curr_link->target_document);
//	char *content = sys_file::read_entire_file(doc.c_str());
////	char *text = sys_file::read_entire_file(corpus_txt::instance().id2docpath(curr_link->target_document).c_str());
//	//char *text = xml2txt::instance().convert(content);
//	char *text = xml2txt::instance().gettext(curr_link->target_document, content);
//	char *textcopy = strdup(text);
//
//	char title[1024];
//	char term[1024];
//	strcpy(term, a_topic->get_name());
//	get_doc_name(content, title);
//	string_tolower(title);
//	string_tolower(textcopy);
//	string_tolower(term);
//
//	curr_link->link_term = new ANT_link_term;
//	curr_link->link_term->postings_length = 1;
//	curr_link->link_term->postings = new ANT_link_posting[curr_link->link_term->postings_length];
//	curr_link->link_term->total_occurences = 0;
//
//	// get the anchor
//	char *where = NULL;
//	long title_offset = strlen(title) + 2;
//	char *source = textcopy + title_offset;
//	long offset = 0;
//	int term_len = find_phrase(source, term, &offset);
//	//long offset = where - source;
//	curr_link->offset = title_offset + offset;
//	//free(curr_link->term);
//	curr_link->term = strndup(text + curr_link->offset, term_len);
//
//	// debug
////	if (curr_link->target_document == 214711)
////		puts("I got you");
//
//	// get the bep
//	curr_link->link_term->postings->offset = bep_algor_->find_anchor_and_beps(a_topic, title);
//	//free(text);
//	delete [] content;
//	delete [] text;
//	free(textcopy);
//}
