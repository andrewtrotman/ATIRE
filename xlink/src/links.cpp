/*
 * links.cpp
 *
 *  Created on: Aug 16, 2009
 *      Author: monfee
 */

#include "links.h"
#include "cmdline_options.h"
#include "sys_file.h"
#include "ltw_topic.h"
#include "ant_link_term.h"
#include "ant_link_posting.h"
#include "corpus.h"
#include "corpus_txt.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>

using namespace QLINK;

namespace QLINK {
	class link_equal {
	private:
		const char *term_to_find;

	public:
		link_equal(const char *term) : term_to_find(term) {}
		~link_equal() {}

		bool operator()(link* other) const
		{
			return *other == term_to_find;
		}

	//    bool operator()(link& other) const
	//    {
	//        return *to_find == other;
	//    }
	};
}

links::links() : topic_(NULL)
{
	init();
}

links::links(ltw_topic *a_topic) : topic_(a_topic)
{
	init();

	if (topic_->get_ltw_task()->is_f2f_task()) {
		links_to_print_ = 250;
	}
}

links::~links()
{
	//delete [] links_in_orphan;
	//if (!link_managed_) {
		int i, j;
		for (i = 0; i < links_in_orphan_.size(); i++) {
			delete links_in_orphan_[i];
			links_in_orphan_[i] = NULL;
		}

//		if (to_clean_)
//			for (j = 0; j < all_links_in_file_.size(); j++) {
//				if (all_links_in_file_[j]->link_term) {
//					delete all_links_in_file_[j]->link_term;
//					all_links_in_file_[j]->link_term = NULL;
//				}
//			}

		for (i = 0; i < all_links_in_file_.size(); i++) {
			delete all_links_in_file_[i];
			all_links_in_file_[i] = NULL;
		}
		//delete [] all_links_in_file;
	//}
	all_links_in_file_.clear();
	links_in_orphan_.clear();
}

void links::init()
{
//	to_clean_ = false;
//	link_managed_ = false;
	links_to_print_ = 0;
	beps_to_print_ = 0;
	mode_ = MODE_NO_4_DIGIT_NUMBERS;
	bep_algor_ = NULL;
	//this->init_links();
}

//void links::init_links()
//{
//	links_in_orphan_ = new link[MAX_LINKS_IN_FILE];
//	all_links_in_file = new link[MAX_LINKS_IN_FILE];
//	all_links_in_file_.size() = 0;
//	links_in_orphan__length = 0;
//}

QLINK::link *links::create_new_link()
{
	link *new_link = link_maker();
	all_links_in_file_.push_back(new_link);
	return new_link;
}

QLINK::link *links::create_new_orphan_link()
{
	link *new_link = link_maker();
	links_in_orphan_.push_back(new_link);
	return new_link;
}

void links::print_links(long orphan_docid, const char *orphan_name, long links_to_print, long max_targets_per_anchor, long mode)
{
//	long *links_already_printed = NULL;
//	long current, links_already_printed_length, links_printed;
//	long result, current_anchor, forget_it, has_link, anchors_printed;
//	//char *orphan_name = "Unknown";
//	const long links_all = links_to_print * max_targets_per_anchor;
//	const long array_size = links_all * 2;
//	long stop_sign = 0;
//
//	//long links_already_printed_array[links_to_print * max_targets_per_anchor * 2]; // = new long [array_size];
//	//links_already_printed = links_already_printed_array;
//	links_already_printed = new long [array_size];
//
//	#ifdef REMOVE_ORPHAN_LINKS
//	links_already_printed[0] = orphan_docid;		// fake having already printed the oprhan itself.
//	links_already_printed_length = 1;
//	#endif
//
//	result = 0;
//	links_printed = 0;
//
//	print_link_tag_header();
//	if (type() == OUTGOING_LINK_TYPE) {
//		while ((result < all_links_in_file_.size()) && (links_printed < links_to_print))
//			{
//			if (strlen(all_links_in_file_[result]->link_term->term) == 0)
//				{
//				result++;		// in the case of empty term
//				continue;
//				}
//
//			if (mode & MODE_NO_4_DIGIT_NUMBERS)
//				if ((strlen(all_links_in_file_[result]->link_term->term) == 4) && (atol(all_links_in_file_[result]->link_term->term) > 999))
//					{
//					result++;		// in the case of a 4 digit number we ignore as we're probably a year
//					continue;
//					}
//
//			has_link = FALSE;
//			anchors_printed = current_anchor = 0;
//			while ((current_anchor < all_links_in_file_[result]->link_term->postings_length) && (anchors_printed < max_targets_per_anchor))
//				{
//				forget_it = FALSE;
//				for (current = 0; (current < links_already_printed_length) && (current < array_size); current++)
//					if (links_already_printed[current] == all_links_in_file_[result]->link_term->postings[current_anchor].docid)
//						forget_it = TRUE;
//				if (!forget_it)
//					{
//					links_already_printed[links_already_printed_length] = all_links_in_file_[result]->link_term->postings[current_anchor].docid;
//					if (links_already_printed_length > 1)
//						printf(", ");
//					printf("%d", all_links_in_file_[result]->link_term->postings[current_anchor].docid);
//					anchors_printed++;
//					links_already_printed_length++;
//					if (/*anchors_printed >= max_targets_per_anchor || */links_already_printed_length >= links_all)
//						{
//						stop_sign = 1;
//						break;
//						}
//					}
//				current_anchor++;
//				}
//			result++;
//
//			if (stop_sign)
//				break;
//			} // while
//	} else if (type() == INCOMING_LINK_TYPE) {
//		while ((result < all_links_in_file_.size()) && (links_printed < links_all)) {
//			if (strlen(all_links_in_file_[result]->term) == 0) {
//				result++;		// in the case of empty term
//				continue;
//			}
//			if (links_printed > 0)
//				printf(", ");
//			printf("%d", all_links_in_file_[result]->target_document);
//			links_printed++;
//			result++;
//		}
//	}
//	print_link_tag_footer();
//
//	delete [] links_already_printed;
}

void links::print()
{
	print_header();
	//print_link_tag_header();
	if (topic_->get_ltw_task()->is_f2f_task()) {
		print_links(topic_->get_id(), topic_->get_name());
	}
	else if (topic_->get_ltw_task()->is_a2b_task()) {
		print_anchors(topic_->get_id(), topic_->get_name());
	}
	//print_link_tag_footer();
	print_footer();
}

void links::print_header()
{
	puts("	<link>\n");
}

void links::print_footer()
{
	puts("	</link>\n");
}

void links::sort_orphan()
{
	//qsort(links_in_orphan_, links_in_orphan_length, sizeof(*links_in_orphan), link::string_target_compare);
	//bsearch(links_in_orphan, links_in_orphan_length, sizeof(*links_in_orphan), link::string_target_compare);
	std::sort(links_in_orphan_.begin(), links_in_orphan_.end(), link_string_target_compare());
}

QLINK::link *links::push_link(char *place_in_file, long offset, char *buffer, long docid, double gamma, ANT_link_term *node)
{
	link *current = create_new_link();

	//current = all_links_in_file + all_links_in_file_length;
	current->place_in_file = place_in_file;
	current->offset = offset;
	current->term = strdup(buffer);
	current->gamma = gamma;
	current->target_document = docid;
	current->link_term = node;

	//all_links_in_file_length++;
	if ((all_links_in_file_.size()) >= MAX_LINKS_IN_FILE)
		exit(printf("Too many links in this file (aborting)\n"));

	return current;
}

QLINK::link *links::push_link(char *buffer, long docid, ANT_link_term *node)
{
//	link *current = 0;
//
//	current = all_links_in_file + all_links_in_file_.size();
//
//	//current->place_in_file = place_in_file;
//	current->term = _strdup(buffer);
//	current->gamma = 0;
//	current->target_document = docid;
//	current->link_term = node;
//
//	all_links_in_file_length++;
//	if ((all_links_in_file_length) >= MAX_LINKS_IN_FILE)
//		exit(printf("Too many links in this file (aborting)\n"));
	return push_link(NULL, 0, buffer, docid, 0, node);
}

QLINK::link *links::push_link(link *lnk)
{
	return push_link(lnk->place_in_file, lnk->offset, lnk->term, lnk->target_document, lnk->gamma, lnk->link_term);
	//return lnk;
}

/*
	DEDUPLICATE_LINKS()
	-------------------
*/
void links::deduplicate_links(void)
{
//link *from = NULL, *to = NULL;
//
//from = to = all_links_in_file + 1;
//
//while (from < all_links_in_file + all_links_in_file_length)
//	{
//	if (strcmp(from->term, (from - 1)->term) != 0)
//		{
//		*to = *from;
//		from++;
//		to++;
//		}
//	else
//		from++;
//	}
//all_links_in_file_length = to - all_links_in_file;

	std::vector<link *>::iterator pre = all_links_in_file_.begin();
	std::vector<link *>::iterator from = pre++;

	while (from != all_links_in_file_.end()) {
		if (strcmp((*from)->term, (*pre)->term) != 0) {
			pre = from;
			from++;
		}
		else {
			delete (*from);
			from = all_links_in_file_.erase(from);
		}
	}
}

void links::sort_links()
{
	int size = all_links_in_file_.size();
	if ( size > 0) {
		// debug
		// before sorting
//		link *link_ptr = NULL;
//		fprintf(stderr, "Total %d links, now sorting\n", all_links_in_file_.size());
//		for (int i = 0; i < all_links_in_file_.size(); i++) {
//			link_ptr = all_links_in_file_[i];
//			printf ("#%d: %s, %f, %d\n", i, link_ptr->term, link_ptr->gamma, link_ptr->target_document);
//		}
		//qsort(all_links_in_file, (size_t)all_links_in_file_length, sizeof(*all_links_in_file), ANT_link::compare);
		//std::sort(all_links_in_file_.begin(), all_links_in_file_.end(), link::compare);
		//deduplicate_links();
		//qsort(all_links_in_file, (size_t)all_links_in_file_length, sizeof(*all_links_in_file), link::final_compare);
		std::sort(all_links_in_file_.begin(), all_links_in_file_.end(), link_compare()/*link::final_compare*/);

		// after sorting
		for (int i = 0; i < all_links_in_file_.size(); i++) {
			link *link_ptr = all_links_in_file_[i];
			fprintf (stderr, "#%d: %s, %f, %d\n", (i + 1), link_ptr->term, link_ptr->gamma, link_ptr->target_document);
		}
	}
}

void links::sort_links_by_term()
{
	std::sort(all_links_in_file_.begin(), all_links_in_file_.end(), term_compare());

	//debug
	for (int i = 0; i < all_links_in_file_.size(); i++) {
		link *link_ptr = all_links_in_file_[i];
		fprintf (stderr, "#%d: %s, %f, %d\n", (i + 1), link_ptr->term, link_ptr->gamma, link_ptr->target_document);
	}
}

QLINK::link *links::find(const char *term)
{
	link *found = NULL;

	std::vector<link *>::iterator iter = find_if(all_links_in_file_.begin(), all_links_in_file_.end(), link_equal(term));

	if (iter != all_links_in_file_.end())
		found = (*iter);
	return found;
}

QLINK::link *links::find_orphan(const char *term)
{
	link *found = NULL;

	std::vector<link *>::iterator iter = find_if(links_in_orphan_.begin(), links_in_orphan_.end(), link_equal(term));

	if (iter != links_in_orphan_.end())
		found = (*iter);
	return found;
}


//void links::find_bep(link *lnk, const char *source)
//{
//	int postings_length = lnk->link_term->postings_length;
//	int count = 0;
//	const char *source = xml2txt
//	for (int j = 0; j < postings_length && count < beps_to_print(); j++) {
//		ANT_link_posting& posting = link->link_term->postings[j];
//		long target_id = posting.docid;
//
//		if (target_id <= 0)
//			continue;
//
////			std::string doc = corpus::instance().id2docpath(target_id);
////			char *content = sys_file::read_entire_file(doc.c_str());
////			char *text = sys_file::read_entire_file(corpus_txt::instance().id2docpath(target_id).c_str());
//		//char *text = xml2txt::instance().convert(content);
//		char *text = xml2txt::instance().gettext(target_id);
//
//		bep_algor_->setsource(link)
//		posting.offset = bep_algor_->find_bep(text, link->link_term->term);
//
//		//delete [] content;
//		delete [] text;
//		count++;
//	}
//}
