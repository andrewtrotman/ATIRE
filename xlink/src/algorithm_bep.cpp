/*
 * algorithm_bep.cpp
 *
 *  Created on: Sep 16, 2009
 *      Author: monfee
 */

#include "algorithm_bep.h"
#include "ltw_topic.h"
#include "ant_link_parts.h"
#include "str.h"

#include <assert.h>

#include <set>
#include <string>
#include <iostream>

using namespace QLINK;
using namespace std;

algorithm_bep::algorithm_bep(std::string& name) : algorithm(NULL)
{
	algor_ind_ = alorithm_names[name];
	source_ = NULL;
	anchor_offset_ = 0;
	strcpy(signature_, "");
}

algorithm_bep::algorithm_bep(ltw_task *task) : algorithm(task)
{

}

algorithm_bep::~algorithm_bep()
{

}

long algorithm_bep::find_bep(ltw_topic *topic, const char *title)
{
	const char *text = topic->get_text();

	return find_bep(text, title);
}


long algorithm_bep::find_bep(const char *text, const char *title)
{
	long offset = 0;
	long length = 0;
	switch (algor_ind_) {
	case BEP_MATCH:
		length = find_phrase(text, title, &offset);
		break;
	case BEP_GRAPH:
		offset = find_bep_in_graph(text);
	case BEP_NONE:
	default:
		break;
	}
	return offset;
}

void algorithm_bep::create_signature()
{
	if (SIGNATURE_SIZE > strlen(source_))
		return;

	const char *start = (anchor_offset_ > SIGNATURE_HALF_SIZE)
							? (source_ + anchor_offset_ - SIGNATURE_HALF_SIZE)
									: source_;

	//start = strfwd(source_, start);

	long offset = start - source_;

	long end_offset = offset + SIGNATURE_SIZE;
	const char *end = NULL;
	if (end_offset > strlen(source_)) {
		end = source_ + strlen(source_);
		start = end - SIGNATURE_SIZE;
	} else {
		end = start + SIGNATURE_SIZE;
		//end = strbck(source_ + strlen(source_), end);
	}
	long size = end - start;
	strncpy(signature_, start, size);
	signature_[size] = '\0';
	signature_set_.clear();
	create_term_set(signature_set_, signature_);
}

void algorithm_bep::create_term_set(std::set<std::string>& term_set, const char *source)
{
	char buffer[MAX_TERM_LENGTH];
	char *copy = strdup(source);
	char **term_list = new char *[strlen(copy)];

	term_set.clear();
	string_tolower(copy);
	long count = string_to_list(copy, term_list);

	for (int i = 0; i < count; i++) {
		strcpy(buffer, "");
		stemmer_.stem(term_list[i], buffer);
		if (term_set.find(buffer) == term_set.end())
			term_set.insert(buffer);
	}

	delete [] term_list;
	free(copy);
}

long algorithm_bep::find_bep_in_graph(const char *text)
{
	if (!source_ )
		return 0;

	if (strlen(signature_) == 0)
		create_signature();

	/**
	 * if the length of the text is pretty much same size with the defined size of signature
	 * then the bep will be the beginning of the text
	 */
	if (strlen(signature_) == 0)
		return 0;

	/*
	 * get the previous 100 characters and following 100 characters
	 */
//	int text_len = strlen(text);
//	int num_of_blocks =  text_len / SIGNATURE_HALF_SIZE;
//
//	if (num_of_blocks < 1)
//		return 0;
//
//	++num_of_blocks;
//	long *offset_array = new long[num_of_blocks];
//	long *match_array = new long[num_of_blocks];
	vector<long> offset_array;
	vector<long> match_array;

	char window[3 * SIGNATURE_HALF_SIZE];

	char *textcopy = strdup(text);
	const char *start = textcopy; // window start
	const char *end = NULL; // window end
	//long count = 0;
	long bep = 0;
	int text_len = strlen(textcopy);
	const char *text_end = textcopy + text_len;
	std::set<std::string> window_term_set;

	string_tolower(textcopy);
	while (start < text_end) {
		if ((text_end - start) > SIGNATURE_SIZE)
			end = start + SIGNATURE_SIZE;
		else
			end = text_end;

		//start = strfwd(textcopy, start);
		//end = strbck(text_end, end);
		int size = end - start;
		strcpy(window, "");
		strncpy(window, start, size);
		window[size] = '\0';
		create_term_set(window_term_set, window);
		offset_array.push_back(start - textcopy);
		match_array.push_back(compare_term_set(signature_set_, window_term_set));

		window_term_set.clear();
		start += SIGNATURE_HALF_SIZE;
		//count++;

		if (offset_array.size() > 2000) {
			cerr << "Warning, the number of blocks is unexpected large" << endl;
			int osize = offset_array.size();
			int ssize = signature_set_.size();
			int wsize = window_term_set.size();
			break;
		}
	}

	int match = match_array[0];
	int where = 0;
	for (int i = 1; i < offset_array.size(); i++)
		if (match_array[i] > match) {
			where = i;
			match = match_array[i];
		}

	bep = offset_array[where];
//	delete [] offset_array;
//	delete [] match_array;
	free(textcopy);
	assert(bep < text_len);
	return bep;
}

int algorithm_bep::compare_term_set(std::set<std::string>& term_set1, std::set<std::string>& term_set2)
{
	int count = 0;

	std::set<std::string>::iterator iter = term_set1.begin();

	for (; iter != term_set1.end(); ++iter)
		if (term_set2.find(*iter) != term_set2.end())
			count++;

	return count;
}
