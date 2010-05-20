/*
 * seg2.cpp
 *
 *  Created on: May 20, 2010
 *      Author: monfee
 */

#include "seg2.h"

Seger2::Seger2(const string_type stream) : Seger(stream)
{
}

Seger2::Seger2(const char* stream, size_t length) : Seger(stream, length)
{
}

Seger2::~Seger2()
{

}

void Seger2::build()
{
	score = 0.0;
	int pos = 0, left = 0;
	assert(freq->array_k_size(k) == 1);
	array_type& char_array = tw_ptr_local_->array();

	for (int i = 1; i < char_array.size(); ++i) {
		int lmin = std::max(0, i - 2);  // i - 2 to i - 1
		int rmax = std::min(char_array.size() - 1, i + 1); // i to i + 1

//		string_type lr_str = tw_ptr_local_->subchars(lmin, i - 1);
//		string_type rr_str = tw_ptr_local_->subchars(i, rmax);
//		word_ptr_type lw = char_array[i - 1];
//		word_ptr_type rw = char_array[i];

//		pos += lw->size();
//		left = len_ - pos;
//		string_type lr_str = lw->chars() + rw->chars();
//		word_ptr_type lr_w = freq->find(lr_str);
//		assert(lr_w != NULL);
		//assert(lr_w->p() != 0.0);

//		if (lr_w->p() == 0.0) {
//			score = std::numeric_limits<double>::max();
//			return;
//		}


			//string_type tmp_str("");
		word_ptr_type tmp_w = NULL;

		//if (lw->size() < rw->size()) {
		//tmp_str = lw->subchars(lw->size() - lmin, lmin)
		//		+ rw->subchars(0, rmin);
		//tmp_w = freq->find(tmp_str);
		tmp_w = tw_ptr_local_->subword(lmin, rmax);

		if (!tmp_w) cerr << "the not found string is "
						<< lmin << " " << rmin
						<< " of " << tw_ptr_local_->chars()
						<< endl;
		assert(tmp_w != NULL);
		//}

		if (tmp_w->lparent()->is_word() && tmp_w->rparent()->is_word()) {
			score += tmp_w->a();
		} {
			if (tmp_w->size() == 3) {
				score += (lmin < rmin) ? tmp_w->left_a() : tmp_w->right_a();
			} else if (tmp_w->size() == 4) {
				score += tmp_w->left_a();
			} else
				score += tmp_w->a();
		}
		boundary_score_.push_back(score);
	}
}

void Seger2::seg()
{


	if (words_list_.size() == 0)
		words_list_.add(tw_ptr_local_);
}

void Seger2::add_to_list(array_type& cwlist)
{

}
