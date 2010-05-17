#include "cwords.h"

#include <algorithm>
#include <iostream>
#include <cassert>
#include <cmath>
#include <limits>

#include "uniseg_settings.h"
#include "uniseg_settings.h"
#include "qfreq.h"
#include "stop_word.h"

using namespace std;

double CWords::highest_s_ = 0.0;
double CWords::lowest_s_ = std::numeric_limits<double>::max();

CWords::CWords(int len, word_ptr_type first) {
	len_ = len;
	pos_ = 0;
	end_ = false;

	insert(first);
	//adjust_pos(first);
	/*
	if (first->size() == len) {
		end_ = true;
		pos_ = len;
	} else
		pos_ = first->size();
		*/
}

CWords::CWords(const CWords& cwords) {
	copy(cwords);
}

CWords::~CWords() {

}

CWords& CWords::operator= (const CWords& cwords) {
    if (this != &cwords) {  // make sure not same object
    	copy(cwords);
    }
    return *this;    // Return ref for multiple assignment

}

void CWords::copy(const CWords& cwords) {
	pos_ = cwords.pos_;
	len_ = cwords.len_;
	end_ = cwords.end_;
	cwords_.clear();
	std::list<word_ptr_type>::const_iterator iit = cwords.words().begin();
	while (	iit != cwords.words().end()) {
		cwords_.push_back((*iit));
		++iit;
	}
	//std::copy(cwords.wordtl list fronts().begin(), cwords.words().end(), cwords_.begin());
	//cerr << "cwords size: " << cwords.words().size() << endl;
	//cerr << "cwords_ size: " << cwords_.size() << endl;
}

void CWords::append(const_word_ptr w_ptr) {
	cwords_.push_back(w_ptr);
	adjust_pos(w_ptr);
}

void CWords::insert(const_word_ptr w_ptr) {
	cwords_.push_front(w_ptr);
	adjust_pos(w_ptr);
}

void CWords::adjust_pos(const_word_ptr w_ptr) {
	pos_ += w_ptr->size();
	if (pos_ == len_)
		end_ = true;
	assert(pos_ <= len_);
}

double CWords::cal(Freq* freq) {
	score_ = 0;
	switch(UNISEG_settings::instance().mean) {
		case 1:
			cal_harmonic_mean(freq);
			break;
		case 2:
			cal_product(freq);
			break;
		case 3:
			cal_geometric_mean(freq);
			break;
		case 4:
			cal_reverse_mi(freq);
			break;
		case 5:
			cal_reverse_mi2(freq);
			break;
		case 6:
			cal_mi(freq, UNISEG_settings::instance().mi);
			break;
		default:
			break;
	}

	return score_;
}

void CWords::cal_harmonic_mean(Freq* freq) {
	Freq& allfreq = QFreq::instance().freq();

	std::list<word_ptr_type>::const_iterator it = cwords_.begin();
	int n = cwords_.size();

	double v = 0.0;
	int size = (*it)->size();
	int k = allfreq.array_size();

	if (size > 0 && size <= k) {
		while (	it != cwords_.end()) {
			//if ((*it)->chars() == "gardenofepicurus")
			//	cerr << "I got you " << endl;

			double wf = static_cast<double>(size * (*it)->freq());
			double sum = static_cast<double>(allfreq.sum_k(size));
			double p = wf/sum;

			//double avg = allfreq.avg_k(size);
			//assert(avg != static_cast<double>(0));
			//double dd = (*it)->freq()/avg;
			//assert(dd != static_cast<double>(0));
			v += 1/p;
			++it;
		}
		assert(v != static_cast<double>(0));
		double H = n/v;
		score_ = H;
	}
}

void CWords::cal_geometric_mean(Freq* freq) {
	cal_product(freq);
	int n = cwords_.size();
	double pr = 1.0 / static_cast<double>(n);
	score_ = powf(score_, pr);
}

void CWords::cal_product(Freq* freq) {
	std::list<word_ptr_type>::const_iterator it = cwords_.begin();
	int n = cwords_.size();

	double v = 1.0;
	int size = (*it)->size();
	int k = QFreq::instance().freq().array_size();
	int count = 0;

	//double sum = static_cast<double>(allfreq.sum_k(1));

	if (size > 0 && size <= k) {
		while (	it != cwords_.end()) {
			//if ((*it)->chars() == "gardenofepicurus")
			//	cerr << "I got you " << endl;

			//double wf = static_cast<double>(size * (*it)->freq());
			//double p = wf/sum;

			//assert(p != static_cast<double>(0));
			count++;
			v *= (*it)->p();
			if (v == static_cast<double>(0))
				break;
			++it;

			//if (count >= (n - 1))
			//	break;
		}
		//assert(v != static_cast<double>(0));
		score_ = v;
	}
	if (score_ > highest_s_)
		highest_s_ = score_;

	if (score_ < lowest_s_)
		lowest_s_ = score_;
}

void CWords::cal_reverse_mi(Freq* freq) {
	if (cwords_.size() < 1) {
		cerr << "No segmentations found for a list" << endl;
		return;
	} else if (cwords_.size() == 1) {
		score_ = std::numeric_limits<double>::max();
		int len = (*cwords_.begin())->size();
		if (len == 2 && (*cwords_.begin())->a() > 0) 			// = 2
			score_ = 0.0;
//		else if (len == 3)	// maybe the = 3
//			score_ = 3.0; //9.0;
//		else 	// >= 4
//			score_ = 4.5; //0.0; //4.5;

		return;
	}


	std::list<word_ptr_type>::const_iterator curr = cwords_.begin();
	std::list<word_ptr_type>::const_iterator next = curr;
	next++;

	score_ = 0.0;
	Freq& allfreq = QFreq::instance().freq();

	while (curr != cwords_.end() && next != cwords_.end()) {
		word_ptr_type lw = (*curr);
		word_ptr_type rw = (*next);
		word_ptr_type lc = (word_ptr_type)lw->rchar();
		word_ptr_type rc = (word_ptr_type)rw->lchar();
		assert(lc != NULL);
		assert(rc != NULL);

		string_type lr_str = lc->chars() + rc->chars();
		word_ptr_type lr_w = freq->find(lr_str);
		assert(lr_w != NULL);
		assert(lr_w->p() != 0.0);
		assert(lc->p() != 0.0);
		assert(rc->p() != 0.0);

		double mi = log(lr_w->p()/(lc->p()*rc->p()));
		double sign = (mi > 0) ? 1.0 : -1.0;

		if (mi < 0)
			assert(sign == -1.0);

		score_ += sign * (mi * mi);

		curr = next;
		next++;
	}

	//score_ = sqrt(score_);
}

void CWords::cal_reverse_mi2(Freq* freq) {
	if (cwords_.size() < 1) {
		cerr << "No segmentations found for a list" << endl;
		return;
	}

	std::list<word_ptr_type>::const_iterator curr = cwords_.begin();

	if (cwords_.size() == 1) {
		if ((*curr)->is_word())
			score_ = std::numeric_limits<double>::min();
		else {
			score_ = std::numeric_limits<double>::max();
			int len = (*curr)->size();
			if (len == 2 && (*curr)->a() > 0) 			// = 2
				score_ = 0.0;
	//		else if (len == 3)	// maybe the = 3
	//			score_ = 9.0;
	//		else 	// >= 4
	//			score_ = 0.0; //4.5;
		}
		return;
	}

	std::list<word_ptr_type>::const_iterator next = curr;
	next++;

	score_ = 0.0;
	Freq& allfreq = QFreq::instance().freq();
	int pos = 0, left = 0;
	int k = freq->array_size();
	assert(freq->array_k_size(k) == 1);
	word_ptr_type whole_input = freq->array_k(k)[0];

	while (curr != cwords_.end() && next != cwords_.end()) {
		word_ptr_type lw = (*curr);
		word_ptr_type rw = (*next);

		pos += lw->size();
		left = len_ - pos;
		string_type lr_str = lw->chars() + rw->chars();
		word_ptr_type lr_w = freq->find(lr_str);
		assert(lr_w != NULL);
		//assert(lr_w->p() != 0.0);

		if (lr_w->p() == 0.0) {
			score_ = std::numeric_limits<double>::max();
			return;
		}

		if (UNISEG_settings::instance().mi <= 0) {
				double mi = log(lr_w->p()/(lw->p()*rw->p()));
				double sign = (mi > 0) ? 1.0 : -1.0;

				if (mi < 0)
					assert(sign == -1.0);

				score_ += sign * (mi * mi);
		}
		else {
			int lmin = std::min(pos, 2);
			int rmin = std::min(left, 2);
			//string_type tmp_str("");
			word_ptr_type tmp_w = NULL;

			//if (lw->size() < rw->size()) {
			//tmp_str = lw->subchars(lw->size() - lmin, lmin)
			//		+ rw->subchars(0, rmin);
			//tmp_w = freq->find(tmp_str);
			tmp_w = whole_input->subword(pos - lmin, lmin + rmin);

			if (!tmp_w) cerr << "the not found string is "
							<< lmin << " " << rmin
							<< " of " << whole_input->chars()
							<< endl;
			assert(tmp_w != NULL);
			//}

			if (tmp_w->lparent()->is_word() && tmp_w->rparent()->is_word()) {
				score_ += tmp_w->a();
			} {
				if (tmp_w->size() == 3) {
					score_ += (lmin < rmin) ? tmp_w->left_a() : tmp_w->right_a();
				} else if (tmp_w->size() == 4) {
					score_ += tmp_w->left_a();
				} else
					score_ += tmp_w->a();
			}
		}

		curr = next;
		next++;
	}

	//score_ = sqrt(score_);
}

void CWords::cal_mi(Freq* freq, int switcher) {
	std::list<word_ptr_type>::const_iterator it = cwords_.begin();
	while (	it != cwords_.end()) {
		if ((*it)->size() == 2) {
			word_ptr_type lc = (word_ptr_type)(*it)->rchar();
			word_ptr_type rc = (word_ptr_type)(*it)->lchar();
			assert(lc != NULL);
			assert(rc != NULL);

			double mi = 0.0;

			switch(switcher) {
			case 0:
				mi = log((*it)->p() / (lc->p() * rc->p()));
				break;
			case 1:
				mi = 0.39 * log((*it)->p()) - 0.28 * log(lc->p()) - 0.23 * log(rc->p()) - 0.32 + log(QFreq::instance().freq().sum_k(1));
				//mi = 0.39 * log((*it)->freq()) - 0.28 * log(lc->freq()) - 0.23 * log(rc->freq()) - 0.32; // + log(QFreq::instance().freq().sum_k(1));
				break;
			case 2:
			{
				break;
			}
			default:
				break;
			}
			score_ += mi;
		}
		it++;
	}
}

void CWords::add_reward_or_penalty(Freq* freq){
	if (UNISEG_settings::instance().reward)
		get_reward_or_penalty(freq);
}
/***
 * we are in faver of longer word in segmentation
 */
void CWords::get_reward_or_penalty(Freq* freq) {
	if (score_ == 0.0)
		return;

	int k = QFreq::instance().freq().array_size();
	double n = static_cast<double>(cwords_.size());

	switch (UNISEG_settings::instance().mean) {
	case 1:
	case 3:
	{

		double v = 0.0;
		double cons_v = 0.0;
		double cons_p = 1.16;

		if (UNISEG_settings::instance().lang == uniseg_encoding::ALPHA)
			cons_v = sqrt(2 * n);
		else if (UNISEG_settings::instance().lang == uniseg_encoding::CHINESE)
			cons_v = 9.0;

		if (n > 0)
			score_ += (cons_v - pow(n, cons_p));
		break;
	}
	case 2:
		{
			/** new code **/
			int ipr = 2;
			std::list<word_ptr_type>::const_iterator it = cwords_.begin();

			int tmp;
			while (	it != cwords_.end()) {
				tmp = (*it)->size();
				if (tmp > k)
					break; // no need to do anything

				if (tmp > ipr && tmp <= k)
					ipr = tmp;

				++it;
			}
			int num_w = cwords_.size();
			int prefer_n = (len_ + 1)/2; //len_ > 3 ? (len_ + 1)/2 : 1;

			if (ipr > 2/*&& prefer_n != n*/) {
				if (UNISEG_settings::instance().debug)
					cerr << "Discriminate " << to_string() << endl;
				/*ipr = len_/num_w + 1;
				if (ipr > len_)
					ipr = len_;*/
				ipr = (ipr + 1)/2;

				//double pr1 = static_cast<double>(ipr);
				double pr2 = pow(lowest_s_/highest_s_, 1.0/static_cast<double>(len_ - 1));

				assert(ipr > 0);
				score_ = score_ * pow(pr2, ipr);
			}
		}
		/** end **/
		break;

	default:
		break;
	}
}

array_type CWords::to_array() {
	array_type temp(0);
	if (cwords_.size() > 0) {

		std::list<word_ptr_type>::const_iterator it = cwords_.begin();

		while (	it != cwords_.end()) {
			temp.push_back((*it));
			++it;
		}

	}
	return temp;
}

bool CWords::reach_limit() {
	return static_cast<double>(size()) > sqrt(static_cast<double>(len_ * 2));
	/*
	if (len_ <= 12) {
		return (size() > ((len_ + 1)/2));
	} else if (len_ <= 16) {
		return size() > (sqrt(len_) + 1);
	}
	return size() > sqrt(len_);
	*/
}

string_type CWords::to_string() {
	string_type str("");
	std::list<word_ptr_type>::const_iterator it = cwords_.begin();

	while (	it != cwords_.end()) {
		str.append((*it)->chars());
		str.append(" ");
		++it;
	}
	return str;
}

int CWords::chinese_stop_word_count()
{
	int count = 0;
	std::list<word_ptr_type>::const_iterator it = cwords_.begin();

	while (	it != cwords_.end()) {
		if ((*it)->size() == 1 && StopWord::is_chinese_stop_word((*it)->chars().c_str()))
			++count;
		++it;
	}
	return count;
}
