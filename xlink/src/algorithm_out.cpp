/*
 * algorithm_out.cpp
 *
 *  Created on: Sep 6, 2010
 *      Author: monfee
 */

#include "algorithm_out.h"
#include "ant_link_term.h"
#include "ant_link_parts.h"
#include "ltw_topic.h"

#include "ltw_task.h"

#include <iostream>

using namespace std;

namespace QLINK
{

algorithm_out::algorithm_out(ltw_task *task) : algorithm(task)
{

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

void algorithm_out::recommend_anchors(links* lx, char **term_list, const char *source)
{
	char **first, **last;
	char *where_to;
	long is_substring, cmp, offset;
	char buffer[1024 * 1024];
//		page_map_t::iterator index_entry = names_map_.end();
//		page_map_t::iterator last_index_entry = index_entry;
	ANT_link_term *index_term = NULL;

	for (first = term_list; *first != NULL; first++)
		{
//			fprintf(stderr, "%s\n", *first);
		current_term_ = *first;
		current_index_ = first - term_list;
		where_to = buffer;
		for (last = first; *last != NULL; last++)
			{
			if (where_to == buffer)
				{
				strcpy(buffer, *first);
				where_to = buffer + strlen(buffer);
				}
			else
				{
				if (segmented_) // segmented text, so we only match the words
					break;

				if (!use_utf8_token_matching_)
					*where_to++ = ' ';
				strcpy(where_to, *last);
				where_to += strlen(*last);
				}

			*where_to = '\0';

//				for the possible debugging later
//				static char di[] = {(char)0xe6, (char)0xa2, (char)0x85};
//				static char xianjin[] = {(char)0xe3, (char)0x80, (char)0x8a, (char)0xe7, (char)0x8e, (char)0xb0};
//				if (memcmp(buffer, xianjin, 6) == 0)
//					fprintf(stderr, "I got you");
//				if (strncmp(*last, "\"", 1)) == 0)
//					fprintf(stderr, "I got you");

//			if (translate_anchor_for_linking_) {
//
//			}
//			else
				index_term = find_term_in_list(buffer);

			if (index_term == NULL)
				break;		// we're after the last term in the list so can stop because we can't be a substring

			if (use_utf8_token_matching_)
				{
				is_substring = FALSE;
				cmp = utf8_token_compare(buffer, index_term->term, &is_substring);
				}
			else
				cmp = string_compare(buffer, index_term->term);

			if (cmp == 0)		// we're a term in the list
				{
				if (strcmp("過年", buffer) == 0)
					cerr << " I caught you" << endl;

				if (!links_->find(buffer)) {
					offset = assign_link_term(buffer, term_list);
					add_link(index_term, term_list, offset);
				}
				}
			else
				{
				if (use_utf8_token_matching_)
					cmp = is_substring == TRUE ? 0 : 1;
				else
					cmp = memcmp(buffer, index_term->term, strlen(buffer));
				if  (cmp != 0)
					break;		// we're a not a substring so we can't find a longer term
				}
			}
		}
	// debug
	if (links_->all_links_length() > 0)
		fprintf(stderr, "added %d links\n", links_->all_links_length());
}

long algorithm_out::assign_link_term(char *buffer, char **term_list)
{
	long term_len, offset, index;

	if (!use_utf8_token_matching_) {
		offset = current_term_ - source_;
	}
	else {
		index = current_index_;
		offset = token_address_[index] - source_;
//		strcpy(buffer_, index_term->term);
	}
	term_len = strlen(buffer);
	strncpy(buffer_, offset + current_topic_->get_content(), term_len);

	if (memcmp(buffer_, buffer, term_len) != 0) {
		term_len = current_topic_->get_term_len(offset, buffer, ltw_task_->is_cjk_lang());
		strncpy(buffer_, offset + current_topic_->get_content(), term_len);
	}

	buffer_[term_len] = '\0';

	if (offset == 3443)
		cerr << " I caught you" << endl;
	return offset;
}

}
