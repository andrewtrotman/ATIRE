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
#include <iterator>

#include "uniseg_settings.h"

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
	//freq_->cal_word_p(freq_stat_->sum_k(1));
	freq_->cal_word_ngmi_a(base_);
	//freq_->show_p();
}

void Seger2::build()
{
	double score = 0.0;
	int pos = 0, left = 0;
	//assert(freq->array_k_size(k) == 1);
	const array_type& char_array = tw_ptr_local_->array();
	boundary_score_.clear();
	boundary_score_.resize(char_array.size() - 1, std::numeric_limits<double>::max());

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
		if (UNISEG_settings::instance().with_training_info ) {
			if (tmp_w->has_word_pair()) {
				boundary_score_[lmin + tmp_w->left()->size() - 1] = -std::numeric_limits<double>::max();
				i += 1;
				continue;
			}
			else if (tmp_w->is_word()) {
				if ((i - 2) > 0)
					boundary_score_[i - 2] = std::numeric_limits<double>::max();
				i += 1;
				continue;
			}
		}
//		if (tmp_w->is_word()) {
//			i += 1;
//			continue;
//		}

		if (!tmp_w) cerr << "the not found string is "
						<< lmin << " " << rmax
						<< " of " << tw_ptr_local_->chars()
						<< endl;
		assert(tmp_w != NULL);
		//}

// 		if (tmp_w->left()->is_word() && tmp_w->right()->is_word()) {
//			score = (tmp_w->a() * tmp_w->a());
//		}
//		else{
			int len = tmp_w->size();

			if (len < 4)
				tmp_w->cal_ngmi_a(i - lmin);

			if (len == 3) {
				score = ((i - lmin) < 2) ? tmp_w->left_a() : tmp_w->right_a();
			} else if (len == 4) {
				score = tmp_w->left_a();
			} else
				score = tmp_w->a();
//		}
		boundary_score_[i - 1] = score;
	}
}

void Seger2::seg()
{
	int last = 0, i = 0;
	word_ptr_type word = NULL;
	for (; i < boundary_score_.size(); ++i) {
		if (boundary_score_[i] <= UNISEG_settings::instance().threshold) {// should be boundary
			word = tw_ptr_local_->subword(last, i + 1 - last);
			words_list_.push_back(word);
			last = i + 1;
		}
	}
	if (last < tw_ptr_local_->size()) {
		word = tw_ptr_local_->subword(last, tw_ptr_local_->size() - last);
		words_list_.push_back(word);
	}

	if (words_list_.size() == 0)
		words_list_.push_back(tw_ptr_local_);

	if (UNISEG_settings::instance().debug) {
		cerr << "threshold: " << UNISEG_settings::instance().threshold << endl;
		cerr << "  ";
		copy (boundary_score_.begin(), boundary_score_.end(), ostream_iterator<double>(cerr, "  "));
		cerr << endl;
		for (i = 0; i < words_list_.size(); ++i)
			cerr << words_list_[i]->chars() << "  ";
		cerr << endl;
	}
}

void Seger2::add_to_list(array_type& cwlist)
{

}

void Seger2::find_boundary(std::vector<double>::iterator begin, std::vector<double>::iterator end, word_ptr_type current_word, std::string& to_become)
{
	std::vector<double>::iterator pos = min_element(begin, end);
	word_ptr_type tmp_word = current_word;

	if (current_word->chars() == "不同的紙")
		cerr << "stop here" << endl;

	string right_left;
	while ((end - begin) > 1) {
		word_ptr_type l_word = tmp_word->subword(0/*begin - boundary_score.begin() - word_count*/, pos - begin + 1);
		word_ptr_type r_word = tmp_word->subword(pos - begin + 1, end - pos);

		if (l_word->is_word() && r_word->is_word()
				|| (r_word->is_candidate_word() && l_word->is_candidate_word())) {
			to_become.append(l_word->chars() + "  " + r_word->chars());
	//									begin = end = pos;
			tmp_word = NULL;
			break;
		}
		else if (l_word->size() > 1 && (l_word->is_word() || l_word->is_candidate_word())) {
			to_become.append(l_word->chars() + "  ");
			tmp_word = r_word;
			begin = pos + 1;
		}
		else if (r_word->size() > 1 && (r_word->is_word() || r_word->is_candidate_word())) {
			right_left.insert(0, string("  ") + r_word->chars());
			tmp_word = l_word;
			end = pos;
		}
		else {
//			if (l_word->has_word_pair()) {
//				to_become.append(l_word->left()->chars() + "  " + l_word->right()->chars() + "  ");
//				begin = pos + 1;
//				tmp_word = r_word;
//			}
//			else if (r_word->has_word_pair()){
//				right_left.insert(0, string("  ") + r_word->left()->chars() + "  " + r_word->right()->chars());
//				end = pos;
//				tmp_word = l_word;
//			}
//			else {
	//										if (l_word->size() < r_word->size()) {
	//										to_become.append(l_word->chars() + "  ");
	//										begin = pos + 1;
	//									}
	//									else {
	//										right_left.insert(0, string("  ") + r_word->chars());
	//										end = pos;
				string left_left;
				Seger::get_rightmost_word_segmentation(l_word, left_left);
//				find_boundary(begin, pos, l_word, to_become);
				to_become.append(left_left);
				string left_right;
//				find_boundary(pos + 1, end, r_word, left_right);
				Seger::get_rightmost_word_segmentation(r_word, left_right);
				left_right.insert(0, string("  "));
				right_left.insert(0, left_right);
	//										begin = end = pos;
				tmp_word = NULL;
				break;
//			}
		}

		pos = min_element(begin, end);
	}
	if (tmp_word != NULL) {
		if (tmp_word->is_word())
			to_become.append(tmp_word->chars());
		else {
			/**
			 * TODO check possible OOV here or somewhere else
			 */
			string left_right;
			Seger::get_rightmost_word_segmentation(tmp_word, left_right);
			to_become.append(left_right);
		}
	}
	//	to_become.append(current_word->subword(begin - boundary_score.begin() - word_count, end - begin + 1)->chars());
	to_become.append(right_left);
}
