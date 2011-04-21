/*
 * algorithm_out.cpp
 *
 *  Created on: Sep 6, 2010
 *      Author: monfee
 */

#include "algorithm_out.h"
#include "ant_link_term.h"
#include "ant_link_parts.h"

#include "ltw_task.h"

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
	long is_substring, cmp;
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
					add_link(index_term, term_list);
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

long algorithm_out::assign_link_term(ANT_link_term *index_term, char **term_list)
{
	long term_len, offset, index;
	term_len = strlen(index_term->term);
	if (!use_utf8_token_matching_) {
		offset = current_term_ - source_;
		strncpy(buffer_, offset + text_, term_len);
		buffer_[term_len] = '\0';
	}
	else {
		index = current_index_;
		offset = token_address_[index] - source_;
		strcpy(buffer_, index_term->term);
	}
	return offset;
}

}
