/*
 * clist.cpp
 *
 *  Created on: Dec 13, 2008
 *      Author: monfee
 */

#include "clist.h"
#include <iterator>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <limits>

#include "uniseg_settings.h"
#include "uniseg_types.h"

using namespace std;

CList::CList() {
	CWords::h_score(0.0);
	CWords::l_score(std::numeric_limits<double>::max());
}

CList::CList(int n, CWords* cwords) {
	for (int i = 0; i < n; i++) {
		append(cwords);
	}
	assert(n == list_.size());
}

CList::~CList() {
	for (cwords_list::iterator i = list_.begin(); i != list_.end(); ++i) {
		if ((*i) != NULL) {
			delete *i;
			*i = NULL;
		}
	}
	list_.clear();
}

bool CList::initialize() {

}

void CList::clear_node(cwords_list_iterator& it)
{
    CWords* cws_ptr;
    cws_ptr = (*it);
    delete cws_ptr;
    cws_ptr = NULL;
}

void CList::append(CWords* cwords) {
	list_.push_back(new CWords(*cwords));
}

void CList::append(const CList& clist) {
	cwords_list::const_iterator it = clist.list().begin();
	for (; it != clist.list().end(); it++)
		append(*it);
}

void CList::insert(CWords* cwords) {
	list_.push_front(new CWords(*cwords));
}

void CList::insert(const CList& clist) {
	cwords_list::const_iterator it = clist.list().begin();
	for (; it != clist.list().end(); it++)
		insert(*it);
}

CWords* CList::front()  {
	return (list_.size() > 0) ? list_.front() : NULL;
}

CWords* CList::second()  {
	if (list_.size() < 2)
		return NULL;
	cwords_list_iterator begin = list_.begin();
	++begin;
	return (*begin);
}

void CList::cal(Freq* freq) {
	cwords_list::iterator it = list_.begin();
	for (; it != list_.end(); ++it) {
		(*it)->cal(freq);
	}

	it = list_.begin();
	for (; it != list_.end(); ++it) {
		(*it)->add_reward_or_penalty(freq);
	}
}

void CList::show() {
	// for debug
	// showing the the combinations
	int n = 50;/*UNISEG_settings::instance().top();
	if (n < 0)
		n = list_.size();*/

	int count = 0;
	cwords_list::iterator it = list_.begin();
	for (; count++ < n && it != list_.end(); ++it) {
		// for debug
		//cerr << "cwords size: " << (*it)->words().size() << endl;
		cerr << (*it)->score() << ": ";
		std::list<word_ptr_type>::const_iterator iit = (*it)->words().begin();
		while (	iit != (*it)->words().end()) {
			cerr << (*iit)->chars() << "(" << (*iit)->freq() << ")" << " ";
			++iit;
		}
		cerr << endl;
	}
}

void CList::remove_no_end() {
	cwords_list::iterator it = list_.begin();
	for (; it != list_.end();) {
		if (!(*it)->is_end()) {
			it = delete_node(it);
		} else
			++it;
	}
}

void CList::remove(int freq, word_ptr_type tw_ptr) {
	cwords_list::iterator it = list_.begin();
	for (; it != list_.end();) {
		bool to_remove = false;
		std::list<word_ptr_type>::const_iterator iit = (*it)->words().begin();
		while (	iit != (*it)->words().end()) {
			if ((*iit)->freq() == freq) {
				if (tw_ptr != NULL && (*iit)->chars() == tw_ptr->chars()) {
					++iit;
					continue;
				}

				to_remove = true;
				break;
			}
			++iit;
		}

		if (to_remove) {
			it = delete_node(it);
		} else
			++it;
	}
}

void CList::remove_two_consecutive_single_chars() {
	/** I am assuming for English, there is a very slim chance for two
	 * single-character words appear consecutively in a frequency pattern
	 *
	 * but for the ancient Chinese, that could be possible
	 */
	if (UNISEG_settings::instance().lang == uniseg_encoding::ALPHA) {
		cwords_list::iterator it = list_.begin();
		for (; it != list_.end();) {
			bool to_remove = false;
			bool get_ready = false;
			std::list<word_ptr_type>::const_iterator iit = (*it)->words().begin();
			while (	iit != (*it)->words().end()) {
				if ((*iit)->size() == 1) {
					if (get_ready) {
						to_remove = true;
						break;
					}
					get_ready = true;
				}
				else
					get_ready = false;

				++iit;
			}

			if (to_remove) {
				it = delete_node(it);
			} else
				++it;
		}
	}
}

bool CList::cmp_score(CWords *cw1, CWords *cw2) {
	return cw1->score() > cw2->score();
}

bool CList::cmp_score_ascending(CWords *cw1, CWords *cw2) {
	return cw1->score() < cw2->score();
}

void CList::sort(bool desc) {
	if (desc)
		list_.sort(cmp_score);
	else
		list_.sort(cmp_score_ascending);
}

cwords_list_iterator CList::delete_node(cwords_list_iterator it) {
/**
 * original code
 *
  			CWords* cws_ptr;
			cws_ptr = (*it);
			it = list_.erase(it);
			delete cws_ptr;
 */
	clear_node(it);
	//cwords_list_iterator  =
	return list_.erase(it);
}


/********************************* SEGMENTATION RULES **************************************
 *
 * 1. if any freq of substring equals with its parent, it should be discarded
 * 2. remove the segmentation with two consecutive single-character words
 *
 *******************************************************************************************/
void CList::apply_rules() {

	if (UNISEG_settings::instance().optimize) {
		int orig_size = size();
		/**
		 * Rule #1
		 */
		for (int i = 0; i <= UNISEG_settings::instance().to_skip; ++i)
			remove(i);
//		remove(1);
//		remove(0);
		//if (tw_ptr_ != NULL)
		//	remove(tw_ptr_->freq(), tw_ptr_);

		/**
		 * Rule #2, english only
		 */
		remove_two_consecutive_single_chars();

		if (UNISEG_settings::instance().debug)
			cerr << "Removed " << orig_size - size() << " bad segmentations from list" << endl;
	}

}
