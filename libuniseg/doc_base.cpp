/*
 * doc_base.cpp
 *
 *  Created on: May 14, 2009
 *      Author: monfee
 */

#include "doc_base.h"
#include "app_conf.h"

#include <iostream>

using namespace std;

const int DocBase::UNAVAILABlE = -1;
const int DocBase::CLEAN = 0;

DocBase::DocBase(Doc* doc)  : doc_(doc) {
	init();
}

DocBase::~DocBase() {
	if (iofs_.is_open())
		iofs_.close();
}

void DocBase::init() {
	if (AppConf::instance().do_save()) {
		this->EXT_NAME = "txt";
		name(doc_->name());
		path(AppConf::instance().opath() + string(AppConf::instance().separator()));
		wopen();
	}

	if (iofs_.is_open())
		output_ = &iofs_;
	else
		output_ = &cout;

	freq_ = &(QFreq::instance().freq());
	wc_ = 0;
}

void DocBase::seg() {
	cout << "Starting segmentation..." << endl;

	/************************************
	 * PRINT THE WORDS THAT ARE SUPOSED
	 * 				TO BE
	 ************************************/
	/*doc_.reset();
	while (doc_.more()) {

		iterator begin;
		iterator end;

		doc_.next(begin, end);

		while (begin <= end) {
			iterator next = begin;

			cout << (*begin)->to_string();

			if (next != end) {
				++next;
				if ((*begin)->end() != (*next)->begin())
					cout << " ";
			} else {
				cout << endl;
				break;
			}
			begin = next;
		}
	}
	cout << endl;*/

	doc_->reset();
	while (doc_->more()) {
		entity_iterator begin;
		entity_iterator end;

		doc_->next(begin, end);

		int slen = end - begin + 1;

		fil_init(slen);
		if (slen > 1)  {// at least two characters in a sentence
			seg_sentence(begin, end);
			output(begin, end);
		}
		else
			output(begin, end);
	}
}

void DocBase::output(entity_iterator& begin, entity_iterator& end) {
	std::ostream& out = (*output_);

	int count = 0;
	entity_iterator one;
	int len = end - begin + 1;
	assert(fil_.size() == len);

	for (int i = 0; i < fil_.size(); i++) {
		one = begin + i;
		out << (*one)->to_string();
		if ((fil_[i] == 0) ||
				((i + 1) < fil_.size() && fil_[i] != fil_[i + 1]))
			out << " ";
	}
	out << endl;
}

void DocBase::fil_init(unsigned int len) {
	fil_.clear();
	assert(fil_.size() == 0);
	for (int i = 0; i < len; i++) {
		fil_.push_back(CLEAN);
	}
}

unsigned int DocBase::fill(unsigned int left, unsigned int len) {
	++wc_; // the segmented words counter

	unsigned int fil_len = 0;
	for (int i = 0; i < len; i++) {
		int idx = left + i;

		if (idx >= fil_.size())
			break;

		if (fil_[idx] == CLEAN) {
			fil_len++;
			fil_[idx] = wc_;
		} else
			cerr << "the character already been marked as word!" << endl;

	}
	return fil_len;
}

/**
 * find the unsegmented characters string, only return the those with more than one character
 */
pair<int, int> DocBase::get_first_unfil(int idx) {
	int start = UNAVAILABlE;
	int i = idx;
	bool flag = false;
	int end = start;

	while (i < fil_.size()) {

		if (!flag) {
			if(fil_[i] == CLEAN) {
				flag = true;
				start = end = i;
			}
		} else {
			if (fil_[i] == CLEAN)
				end++;
			else {
				if (end == start) {
					flag = false;
					start = end = UNAVAILABlE;
				}
				else
					break;
			}
		}
		i++;
	}

	if (end == start)
		start = end = UNAVAILABlE;

	return make_pair(start, end);
}
