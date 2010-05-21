/*
 * seg2.cpp
 *
 *  Created on: May 20, 2010
 *      Author: monfee
 */

#include "seg2.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <limits>

using namespace std;

Seger2::Seger2()
{
}

Seger2::Seger2(const string_type stream) : Seger(stream)
{
}

Seger2::Seger2(const char* stream, size_t length) : Seger(stream, length)
{
}

Seger2::~Seger2()
{

}

void Seger2::do_some_calculations() {
	//freq_->cal_word_p(allfreq_->sum_k(1));
	freq_->cal_word_ngmi_a(allfreq_->sum_k(1));
	//freq_->show_p();
}

void Seger2::build()
{
	double score = 0.0;
	int pos = 0, left = 0;
	//assert(freq->array_k_size(k) == 1);
	const array_type& char_array = tw_ptr_local_->array();
	boundary_score_.clear();
	boundary_score_.resize(char_array.size(), std::numeric_limits<double>::max());

	for (int i = 1; i < char_array.size(); ++i) {
		int lmin = std::max(0, i - 2);  // i - 2 to i - 1
		int rmax = std::min(int(char_array.size() - 1), i + 1); // i to i + 1

			//string_type tmp_str("");
		word_ptr_type tmp_w = NULL;

		//if (lw->size() < rw->size()) {
		//tmp_str = lw->subchars(lw->size() - lmin, lmin)
		//		+ rw->subchars(0, rmin);
		//tmp_w = freq->find(tmp_str);
		tmp_w = tw_ptr_local_->subword(lmin, rmax - lmin + 1);

		if (!tmp_w) cerr << "the not found string is "
						<< lmin << " " << rmax
						<< " of " << tw_ptr_local_->chars()
						<< endl;
		assert(tmp_w != NULL);
		//}

 		if (tmp_w->left()->is_word() && tmp_w->right()->is_word()) {
			score = (tmp_w->a() * tmp_w->a());
		}
		else{
			int len = tmp_w->size();

			if (len < 4)
				tmp_w->cal_ngmi_a(i - lmin);

			if (len == 3) {
				score = ((i - lmin) < 2) ? tmp_w->left_a() : tmp_w->right_a();
			} else if (len == 4) {
				score = tmp_w->left_a();
			} else
				score = tmp_w->a();
		}
		boundary_score_[i - 1] = score;
	}
}

void Seger2::seg()
{
	int last = 0;
	for (int i = 0; i < boundary_score_.size(); ++i) {
		if (boundary_score_[i] < 0) {// should be boundary
			word_ptr_type word = tw_ptr_local_->subword(last, i + 1 - last);
			words_list_.push_back(word);
			last = i + 1;
		}
	}

	if (words_list_.size() == 0)
		words_list_.push_back(tw_ptr_local_);
}

void Seger2::add_to_list(array_type& cwlist)
{

}
