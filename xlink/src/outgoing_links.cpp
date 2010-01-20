/*
 * outgoing_links.cpp
 *
 *  Created on: Aug 16, 2009
 *      Author: monfee
 */

#include "outgoing_links.h"
#include "outgoing_link.h"
#include "ltw_topic.h"
#include "ant_link_term.h"
#include "ant_link_posting.h"
#include "algorithm_bep.h"
#include "xml2txt.h"
#include "corpus.h"
#include "corpus_txt.h"
#include "sys_file.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace QLINK;
using namespace std;

outgoing_links::outgoing_links(ltw_topic *a_topic) : links(a_topic)
{
	init();
}


outgoing_links::outgoing_links()
{
	links_to_print_ = 250;
	beps_to_print_ = 5;
	init();
}

outgoing_links::~outgoing_links()
{
}


inline QLINK::link *outgoing_links::link_maker() {
	return new outgoing_link;
}

void outgoing_links::init_links()
{
	//links_in_orphan = new outgoing_link[MAX_LINKS_IN_FILE];
//	all_links_in_file = new outgoing_link[MAX_LINKS_IN_FILE];
//
//	for (int i = 0; i < MAX_LINKS_IN_FILE; i++) {
//		links_in_orphan[i] = new outgoing_link;
//	}
	//all_links_in_file_length = 0;
	//links_in_orphan_length = 0;
}

void outgoing_links::init() {
	type(OUTGOING_LINK_TYPE);

	if (topic_->get_ltw_task()->is_a2b_task()) {
		links_to_print_ = 50;
		beps_to_print_ = 5;
	}

	init_links();
}

void outgoing_links::print_header()
{
	puts("	<outgoing>");
}

void outgoing_links::print_footer()
{
	puts("	</outgoing>\n");
}

void outgoing_links::print_links(long orphan_docid, const char *orphan_name, long links_to_print, long max_targets_per_anchor, long mode)
{
	//long *links_already_printed = NULL;
	long current, /*links_already_printed_length, */links_printed;
	long result, current_anchor = 0, forget_it, has_link, anchors_printed;
	//char *orphan_name = "Unknown";
	const long links_all = links_to_print * max_targets_per_anchor;
	const long array_size = links_all * 2;
	long stop_sign = 0;

	outgoing_link *current_link = NULL;

	//long links_already_printed_array[links_to_print * max_targets_per_anchor * 2]; // = new long [array_size];
	//links_already_printed = links_already_printed_array;
	std::vector<long> links_already_printed; // = new long [array_size];

	#ifdef REMOVE_ORPHAN_LINKS
	links_already_printed.push_back(orphan_docid);		// fake having already printed the oprhan itself.
	//links_already_printed_length = 1;
	#endif

	result = 0;
	links_printed = 0;

	print_link_tag_header();

	while ((links_already_printed.size() < links_to_print)) {
		stop_sign = TRUE;

		while ((result < all_links_in_file_.size()) && (links_already_printed.size() < links_to_print)) {
			current_link = reinterpret_cast<outgoing_link *>(all_links_in_file_[result]);
			if (strlen(current_link->link_term->term) == 0)
				{
				result++;		// in the case of empty term
				continue;
				}

			if (mode & MODE_NO_4_DIGIT_NUMBERS)
				if ((strlen(current_link->link_term->term) == 4) && (atol(current_link->link_term->term) > 999))
					{
					result++;		// in the case of a 4 digit number we ignore as we're probably a year
					continue;
					}

			//has_link = FALSE;
			//anchors_printed = current_anchor = 0;
			//fprintf(stderr, "curent term: \"%s\", with posting length %d\n", current_link->link_term->term, current_link->link_term->postings_length);
			if ((current_anchor < current_link->link_term->postings.size()) /*&& (anchors_printed < max_targets_per_anchor)*/) {
				ANT_link_posting& current_posting = *current_link->link_term->postings[current_anchor];
				long current_posting_docid = current_posting.docid;
				if (std::find(links_already_printed.begin(), links_already_printed.end(), current_posting_docid)
					== links_already_printed.end()) {
					long docid = current_link->link_term->postings[current_anchor]->docid;
					std::string docfile = corpus::instance().id2docpath(docid);
					if (sys_file::exist(docfile.c_str())) {
						links_already_printed.push_back(docid);
						#ifdef REMOVE_ORPHAN_LINKS
						if (links_already_printed.size() > 2)
						#else
						if (links_already_printed.size() > 1)
						#endif
							printf(", ");
						current_link->print_target(current_anchor);
						fprintf(stderr, "%s (gamma = %f)\n", current_link->link_term->term, current_link->gamma);
					}
					else
						cerr << "No such file:" << docfile << endl;
					//anchors_printed++;
					//links_already_printed.size()++;
//					if (/*anchors_printed >= max_targets_per_anchor || */links_already_printedlength >= links_all)
//						{
//						stop_sign = 1;
//						break;
//						}
					stop_sign = false;
					//has_link = TRUE;
				}
				//current_anchor++;
			} // if current_anchor
			result++;
			//if (has_link) links_printed++;
		} // iner while


		if (stop_sign)
			break;

		current_anchor++;
		result = 0;
	}

	print_link_tag_footer();

	//delete [] links_already_printed;
}

void outgoing_links::print_anchors(long orphan_docid, const char *orphan_name)
{
	long current = 0, links_printed = 0;
	long result = 0;

	outgoing_link *current_link = NULL;

	result = 0;
	links_printed = 0;

	//print_link_tag_header();

	while ((result < all_links_in_file_.size()) && (links_printed < links_to_print_)) {
		current_link = reinterpret_cast<outgoing_link *>(all_links_in_file_[result]);
		if (strlen(current_link->link_term->term) == 0)
			{
			result++;		// in the case of empty term
			continue;
			}

		if (mode_ & MODE_NO_4_DIGIT_NUMBERS)
			if ((strlen(current_link->link_term->term) == 4) && (atol(current_link->link_term->term) > 999))
				{
				result++;		// in the case of a 4 digit number we ignore as we're probably a year
				continue;
				}

		if (topic_->get_ltw_task()->get_task_type() != ltw_task::LTW_LTAra_A2B) {
			current_link->print_anchor(beps_to_print_);
			links_printed++;
		}
		else {
			std::vector<ANT_link_posting *>& postings = current_link->link_term->postings;
			int i = 0;
			int stop = -1;
			for (; i < postings.size(); i++) {
				if (strcmp(topic_->get_name(), postings[i]->desc) == 0) {
					stop = i;
					break;
				}
			}
			int ret = i - 1;
			if (stop == -1 || (stop > -1 && (postings.size() - 1) > 0)) {
				int count = 0;
				printf("\t\t\t<anchor offset=\"%d\" length=\"%d\" name=\"%s\">\n", current_link->offset, strlen(current_link->term), current_link->term);
				for (i = 0; i < postings.size(); i++) {
					if (i == stop)
						continue;
					printf("\t\t\t\t<tobep offset=\"%d\">%s</tobep>\n", postings[i]->offset, postings[i]->desc);
					++count;
					if (count >= beps_to_print_)
						break;
				}
				puts("\t\t\t</anchor>\n");
			}
		}
		result++;
	} // while

	//print_link_tag_footer();

}

void outgoing_links::find_anchor()
{
	if (!bep_algor_)
		return;

	int size = all_links_length();
	int link_count = 0;
	for (int i = 0; i < size && link_count < links_to_print_; i++) {
		link *lnk = all_links()[i];

		if (lnk->link_term->term && strlen(lnk->link_term->term) == 0)
				continue;

		// debug
		if (i == 189)
			fprintf(stderr, "%s\n", lnk->link_term->term);

		link_count++;
		int postings_length = lnk->link_term->postings.size();
		int count = 0;
		for (int j = 0; j < postings_length && count < beps_to_print(); j++) {
			ANT_link_posting& posting = *lnk->link_term->postings[j];
			long target_id = posting.docid;

			if (target_id <= 0 && topic_->get_ltw_task()->get_task_type() != ltw_task::LTW_LTAra_A2B)
				continue;

//			std::string doc = corpus::instance().id2docpath(target_id);
//			char *content = sys_file::read_entire_file(doc.c_str());
//			char *text = sys_file::read_entire_file(corpus_txt::instance().id2docpath(target_id).c_str());
			//char *text = xml2txt::instance().convert(content);
			char *text = NULL;
			if (topic_->get_ltw_task()->get_task_type() != ltw_task::LTW_LTAra_A2B)
				text = xml2txt::instance().gettext(target_id);
			else
				text = xml2txt::instance().gettearatext(posting.desc);

			if (!text) {
				cerr << "No existing target id(" << target_id << ") desc(" << (!posting.desc ? "" : posting.desc) << ")found" << endl;
				posting.docid = 0 - posting.docid;
				continue;
			}

			int len = strlen(text);

			if (bep_algor_) {
				bep_algor_->setsource(topic_->get_text());
				bep_algor_->set_anchor_offset(lnk->offset);
				long bep = bep_algor_->find_bep(text, lnk->link_term->term);
				posting.offset = bep;
			}

			//delete [] content;
			delete [] text;
			count++;
		}
	}
}
