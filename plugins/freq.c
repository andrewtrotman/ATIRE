/*
 * freq.cpp
 *
 *  Created on: Oct 26, 2008
 *      Author: monfee
 */

#include "freq.h"
#include "utilities.h"
#include "uniseg_settings.h"
#include "uniseg_types.h"

#include <iostream>
#include <algorithm>

using namespace std;

class NeedSkip {    // function object that returns true for the call
  private:
    int to_skip_;     // to_skip

  public:
	NeedSkip (int to_skip) : to_skip_(to_skip) {
    }
    bool operator() (word_ptr_type w_ptr) {
        return w_ptr->freq() <= to_skip_;
    }
};

Freq::Freq() : sum_n_(UNISEQ_settings::MAX_CHARS, 0), avg_n_(UNISEQ_settings::MAX_CHARS, 0), k_(0) {
	freq_n_.push_back(freq_1_);  // freq_n_[0]
	for (int i = 0; i < UNISEQ_settings::MAX_CHARS; i++) {
		//array_type freq_n;
		freq_n_.push_back(array_type());
	}
}

Freq::~Freq() {
	freq_type::const_iterator iter;
    for (iter=freq_.begin(); iter != freq_.end(); ++iter)
    	delete iter->second;
}

/** TODO
 *  This function needs to be rewritten, because the Word* cannot be shared
 *  a new Word* needs to be created instead
 */
void Freq::clone(Freq& freq) {
	freq.k_ = k_;
	for (int i = 0; i < freq_n_.size(); i++)
		std::copy(freq_n_[i].begin(), freq_n_[i].end(), freq.freq_n_[i].begin());

	freq.freq_.insert(freq_.begin(), freq_.end());
}

bool Freq::has_word(string_type word) {
	return (freq_.find(word) != freq_.end());
}

word_ptr_type Freq::find(string_type word) {
	freq_type::const_iterator it = freq_.find(word);
	if (it != freq_.end())
		return it->second;
	return NULL;
}

word_ptr_type Freq::add(string_array& ca,  unsigned int freq) {
	string_type chars = array_to_string<string_array, string_type>(ca);
	int size = ca.size();
	assert(size > 0);

	word_ptr_type word_ptr = find(chars);

	/*
	if (UNISEQ_settings::instance().load && size <= UNISEQ_settings::instance().min) {
		/// for debug
		if (word_ptr) {
			cout << "array size: " << freq_n_.size() << endl;
			cout << "chars: " << chars << endl;
		}
		assert(!word_ptr);
	}
	*/

	if (word_ptr != NULL) {
		if (freq == 0)
			word_ptr->increase();
		else
			word_ptr->freq(freq);
	}
	else {
		//cout << "adding new word : " << chars << endl;
		bool skip = false;

		if (UNISEQ_settings::instance().load) {
			if (freq > 0 && size > 1) {
				if (freq <= UNISEQ_settings::instance().to_skip)
					skip = true;

				if(!skip && UNISEQ_settings::instance().skipit(size, freq))
					skip = true;
			}
		} // loading

		// for debug
		//if (skip && size > 22)
		//	cout << "skipping chars: " << chars << endl;

		if (!skip) {
			if (freq == 0)
				word_ptr = new word_type(chars, size);
			else
				word_ptr = new word_type(chars, freq, size);

			/// make the Word linked to the word which is the sub
			if (word_ptr->size() > 1) {

				string_type lp = array_to_string<string_array, string_type>(ca, 0, size - 1);
				string_type rp = array_to_string<string_array, string_type>(ca, 1, size - 1);
				string_type lc = ca[0];
				string_type rc = ca[size - 1];


				//cout << "getting " << tmp_it->second->chars()
				//		<< " address: " << tmp_it->second->address() << endl;

				word_ptr_type lparent = find(lp);
				word_ptr_type rparent = find(rp);

				if (!lparent || !rparent) {

					// for debug
					//if (size > 22)
					//	cout << "could find parent for chars: " << chars << endl;

					delete word_ptr;
					word_ptr = NULL;
					return word_ptr;
				}

				word_ptr->lparent(lparent); word_ptr->lchar(find(lc));
				word_ptr->rparent(rparent);word_ptr->rchar(find(rc));

				assert(word_ptr->lparent() != NULL);
				assert(word_ptr->rparent() != NULL);
				assert(word_ptr->lchar() != NULL);
				assert(word_ptr->rchar() != NULL);

			} else {
				word_ptr->lchar(word_ptr);
				word_ptr->rchar(word_ptr);
				word_ptr->lparent(NULL);
				word_ptr->rparent(NULL);
			}

			//freq_.insert(make_pair(chars, word_ptr));
			//freq_n_[size].push_back(word_ptr);
			add(word_ptr);

			word_ptr->address(freq_n_[size].size() - 1);
			//word_ptr->size(size);

			assert(word_ptr->size() == size);
			assert(word_ptr->size() == (int)ca.size());

			array_type wa;
			array_to_array(wa, ca);
			word_ptr->array(wa);
		}
		//else
		//	cout << "skipping ..." << endl;
	}

	return word_ptr;
}

void Freq::add(word_ptr_type word_ptr) {
	//if (word_ptr->chars() == "ettoreximenes")
	//	cout << " I got you " << endl;

	assert(word_ptr != NULL);
	if (word_ptr->size() > k_)
		k_ = word_ptr->size();

	freq_.insert(make_pair(word_ptr->chars(), word_ptr));
	freq_n_[word_ptr->size()].push_back(word_ptr);
}

void Freq::sort(int k) {
	cout << endl;
	cout << "now sorting the results" << endl;

	if (freq_n_[k].size() > 0) {
		cout << "total words: " << freq_n_[k].size() << endl;
		remove_low(k);
		cout << "after skipping low: " << freq_n_[k].size() << endl;
		std::sort(freq_n_[k].begin(), freq_n_[k].end(), Word::cmp_freq);
	}

	cout << "finished sorting" << endl;
}

void Freq::show(int n) {
	cout << endl;
	cout << "total arrays #: " << freq_n_.size() << endl;
	cout << "listing words with size : " << n << endl;
	cout << "number of words:" ;
	if (n < (int)freq_n_.size()) {
		cout << freq_n_[n].size() << endl;
		array_type temp_arr;
		int i = 0;
		for (i = 0; i < (int)freq_n_[n].size(); i++)
			temp_arr.push_back(freq_n_[n][i]);
		std::sort(temp_arr.begin(), temp_arr.end(), Word::cmp_just_freq);

		for (i = 0; i < (int)temp_arr.size(); i++)
			cout << temp_arr[i]->chars() << ": " <<  temp_arr[i]->freq() << endl;
	} else
		cout << " 0" << endl;

}

void Freq::show() {
	freq_type::const_iterator iter;
	for (iter=freq_.begin(); iter != freq_.end(); ++iter)
		cout << iter->first << endl;
}

void Freq::alloc(int k) {
	assert(k > 0);

	cout << "calculating address for words with size " << k << endl;

	if ((int)freq_n_[k].size() < 1) {
		cerr << "Please do the frequency calculation for "
			<< k << " characters first" << endl;
		return;
	}

	if ((int)freq_n_[k].size() <= 0)
		return;

	int freq = 0;
	for (int i = 0; i < (int)freq_n_[k].size(); i++) {
		freq_n_[k][i]->address(i);
		freq += freq_n_[k][i]->freq();
	}

	cout << "total freq: " << freq << endl;
}

void Freq::remove_low(int k) {
	if (k == 1)
		return; /// we want to keep all the frequency for word with only one character

	/* Not a good idea to remove those low frequency word in the following way
	for (int i = (freq_n_[k].size() - 1); i >= 0; i++) {
		if (freq_n_[k][i]->freq() <= UNISEQ_settings::instance().to_skip())
			freq_n_[k].erase(freq_n_[k].begin() + i);
		else
			break;
	}
	*/
	array_type::iterator pos = std::remove_if(
			freq_n_[k].begin(), freq_n_[k].end(), NeedSkip(UNISEQ_settings::instance().to_skip));
	freq_n_[k].erase(pos, freq_n_[k].end());
}

bool Freq::need_skip(word_ptr_type w_ptr) {
	//return w_ptr->freq() <= UNISEQ_settings::instance().to_skip;
	return w_ptr->freq() < 2;
}

void Freq::array_to_array(array_type& wa, string_array& ca) {
	for (int i = 0; i < (int)ca.size(); i++) {
		word_ptr_type w_ptr = find(ca[i]);

		//for debug
		//cout << "finding " << ca[i] << endl;
		assert(w_ptr != NULL);
		wa.push_back(w_ptr);
	}
}

void Freq::cal_sum() {
	int k = 0;
	while (k < UNISEQ_settings::MAX_CHARS) {
		int freq = 0;
		for (int i = 0; i < (int)freq_n_[k].size(); i++) {
			freq += freq_n_[k][i]->freq();
		}
		sum_n_[k] = freq;
		k++;
	}
}

void Freq::cal_avg() {
	int k = 0;
	while (k < UNISEQ_settings::MAX_CHARS) {
		if (freq_n_[k].size() > 0)
			avg_n_[k] = static_cast<float>(sum_n_[k])/freq_n_[k].size();
		k++;
	}

	//for debug
	k = 0;
	cout << "average freq of each size of words" << endl;
	while (k < UNISEQ_settings::MAX_CHARS) {
		if (freq_n_[k].size() > 0)
			cout << k << ": " << avg_n_[k] << " (with sum "
				<< sum_n_[k] << " on size " << freq_n_[k].size() << ")"
			    << endl;
		k++;
	}
}

void Freq::assign_freq(Freq& freq) {
	freq_type::const_iterator iter;
	for (iter=freq_.begin(); iter != freq_.end(); ++iter) {
		word_ptr_type tmp_wp = freq.find(iter->second->chars());
		//assert(tmp_wp != NULL);

		int freqc = 1; /// that means if we could not find in table, then it appears at least one
		if (tmp_wp != NULL) {
			freqc = tmp_wp->freq();
			iter->second->base(tmp_wp->base());
		}
//		else {
//				string_array sa = Word::array_to_array(iter->second->array());
//				tmp_wp = freq.add(sa, iter->second->freq());
//		}

		if (UNISEQ_settings::instance().debug)
			cout << "Assigning " << iter->second->chars()
			<< " with frequency " << freqc << ""
			<< endl;
		iter->second->freq(freqc);
	}
}

void Freq::cal_sum_n_avg() {
	cal_sum();
	cal_avg();
}

void Freq::set_seged(Freq& freq, unsigned int freqc) {
	freq_type::const_iterator iter;
	for (iter=freq_.begin(); iter != freq_.end(); ++iter) {
		if (iter->second->freq() == freqc) {
			word_ptr_type tmp_wp = freq.find(iter->second->chars());
			assert(tmp_wp != NULL);

			tmp_wp->seged(true);
		}
	}
}

void Freq::reduce_freq(Freq& freq, word_ptr_type tw_ptr) {
	freq_type::const_iterator iter;
	int freqc = tw_ptr->freq();
	for (iter=freq_.begin(); iter != freq_.end(); ++iter) {
		word_ptr_type tmp_wp = freq.find(iter->second->chars());
		assert(tmp_wp != NULL);

		if (tw_ptr != tmp_wp) {
			if (tmp_wp->freq() < freqc)
				tmp_wp->freq(0);
			else
				tmp_wp->freq(tmp_wp->freq() - freqc);
		}
	}
}

void Freq::add_freq(Freq& freq, int threshold) {

	for (int i = 1; i <= k_; i++) {
		for (int j = 0; j < freq_n_[i].size(); j++) {
			word_ptr_type c_wp = freq_n_[i][j]; // current word pointer
			word_ptr_type tmp_wp = freq.find(c_wp->chars());

			int freqc = c_wp->freq();
			if (freqc > threshold) {
				if (!tmp_wp) {
					string_array sa = Word::array_to_array(c_wp->array());
					tmp_wp = freq.add(sa, freqc);

					assert(tmp_wp != NULL);
					//tmp_wp->cal_p(this->sum_k(1));

					if (UNISEQ_settings::instance().debug)
						cout << "Found new string pattern for the 1 of N+1: " << tmp_wp->chars()
						<< "(" << tmp_wp->freq() << ")"
						<< endl;
				}
				else
					tmp_wp->freq(tmp_wp->freq() + freqc);

			}
		}
	}
}

void Freq::justify(unsigned int min) {
	for (int i = 0; i < freq_n_[k_].size(); i++)
		freq_n_[k_][i]->justify(min);
}

void Freq::cal_word_p(double base) {
//	freq_type::const_iterator iter;
//	for (iter=freq_.begin(); iter != freq_.end(); ++iter)
//		iter->second->cal_p(base);

	for (int i = 1; i <= k_; i++)
		for (int j = 0; j < freq_n_[i].size(); j++)
			freq_n_[i][j]->cal_p(base);

}

void Freq::cal_word_a() {
//	freq_type::const_iterator iter;
//	for (iter=freq_.begin(); iter != freq_.end(); ++iter)
//		iter->second->cal_p(base);

	freq_type::const_iterator iter;
	for (iter=freq_.begin(); iter != freq_.end(); ++iter) {
//		size = iter->second->size();
//		word_ptr_type w_ptr = iter->second;
//		if (size > 2) {
//
//		}
		if (iter->second->size() <= 4)
			iter->second->cal_a();
	}
}

void Freq::show_p() {
	if (UNISEQ_settings::instance().debug) {
		freq_type::const_iterator iter;
		for (iter=freq_.begin(); iter != freq_.end(); ++iter)
			cout << iter->second->chars() << ": " << iter->second->p()
			<< "(" << iter->second->freq() << ")"
			<< endl;
	}

	cout << endl << "The association scores:" << endl;
	if (UNISEQ_settings::instance().debug) {
		freq_type::const_iterator iter;
		for (iter=freq_.begin(); iter != freq_.end(); ++iter)
			cout << iter->second->chars() << ": " << iter->second->a()
			<< "(" << iter->second->freq() << ")"
			<< endl;
	}
}

void Freq::to_array(std::vector<word_ptr_type>& wa) {
	freq_type::const_iterator iter;
	for (iter=freq_.begin(); iter != freq_.end(); ++iter) {
		assert(iter->second != NULL);
		wa.push_back(iter->second);
	}
}


