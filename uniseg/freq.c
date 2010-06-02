/*
 * freq.cpp
 *
 *  Created on: Oct 26, 2008
 *      Author: monfee
 */

#include "freq.h"
#include "convert.h"
//#include "utilities.h"
#include "uniseg_settings.h"
#include "uniseg_types.h"
#include "file.h"

#include <math.h>

#include <iostream>
#include <algorithm>
#include <iterator>
#include <sstream>

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

Freq::Freq() : sum_n_(UNISEG_settings::MAX_CHARS, 0), avg_n_(UNISEG_settings::MAX_CHARS, 0), k_(0) {
	freq_n_.push_back(freq_1_);  // freq_n_[0]
	for (int i = 0; i < UNISEG_settings::MAX_CHARS; i++) {
		//array_type freq_n;
		freq_n_.push_back(array_type());
	}

	k_ = -1;
	loaded_ = false;
	number_of_documents_ = 0;
	current_document_id_ = 0;
}

Freq::~Freq() {
	freq_type::const_iterator iter;
    for (iter=freq_.begin(); iter != freq_.end(); ++iter)
    	delete iter->second;
}

void Freq::set_current_document_id(long id)
{
	//previous_document_id_ = current_document_id_;
	current_document_id_ = id;
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

void Freq::merge(Freq& freq){
	freq_type::iterator iter;
	for (iter = freq.set().begin(); iter != freq.set().end();) {
		word_ptr_type word = find(iter->first);
		if (!word) {
			add(iter->second);
			freq.set().erase(iter++);
		}
		else {
			word->increase(iter->second->freq());
			++iter;
		}
	}
}

void Freq::mergeto(Freq& freq){
	for (int i = 1; i <= k_; i++) {
		array_type::iterator iter = freq_n_[i].begin();
		for (; iter != freq_n_[i].end();) {
			word_ptr_type word = freq.find((*iter)->chars());
			if (!word) {
				string_array ca;
				(*iter)->to_string_array(ca);
				freq.add(ca, (*iter)->lang());
				//iter = freq_n_[i].erase(iter);
			}
			else
				word->increase();

			++iter;
		}
	}
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

word_ptr_type Freq::add(string_array& ca, long lang, unsigned int freq, bool allnew) {

	int size = ca.size();
	assert(size > 0);

	string_type chars; // = array_to_string<string_array, string_type>(ca);

	/*
	 * create key string
	 */
	array_to_string(ca, lang, chars);

	// debug
//	if (chars == "红灯笼" || chars == "\347\272\242\347\201\257\347\254\274")
//		cerr << "got you " << endl;

	word_ptr_type word_ptr = NULL;

	if (!allnew)
		word_ptr = find(chars);

	/*
	if (UNISEG_settings::instance().load && size <= UNISEG_settings::instance().min) {
		/// for debug
		if (word_ptr) {
			cerr << "array size: " << freq_n_.size() << endl;
			cerr << "chars: " << chars << endl;
		}
		assert(!word_ptr);
	}
	*/

	if (word_ptr) {
		if (freq == -1)
			word_ptr->increase();
		else if (freq > 0)
			word_ptr->freq(freq);
	}
	else {
		//cerr << "adding new word : " << chars << endl;
		bool skip = false;

		if (UNISEG_settings::instance().load) {
			if (freq > 0 && size > 1) {
				if (UNISEG_settings::instance().do_skip && freq <= UNISEG_settings::instance().to_skip)
					skip = true;

				if(UNISEG_settings::instance().do_skip && !skip && UNISEG_settings::instance().skipit(size, freq))
					skip = true;
			}
		} // loading

		// for debug
		//if (skip && size > 22)
		//	cerr << "skipping chars: " << chars << endl;

		if (!skip) {
			if (freq == -1)
				word_ptr = new word_type(chars, size);
			else
				word_ptr = new word_type(chars, freq, size);

			word_ptr->lang(lang);
			/// make the Word linked to the word which is the sub
			if (word_ptr->size() > 1) {

				string_type lp;
				array_to_string(ca, lang, lp, 0, size - 1);
				string_type rp;
				array_to_string(ca, lang, rp, 1, size - 1);
				string_type lc = ca[0];
				string_type rc = ca[size - 1];


				//cerr << "getting " << tmp_it->second->chars()
				//		<< " address: " << tmp_it->second->address() << endl;
				if (!allnew) {
					word_ptr_type lparent = find(lp);
					word_ptr_type rparent = find(rp);

//					if (!lparent || !rparent) {
//
//						// for debug
//						//if (size > 22)
//						//	cerr << "could find parent for chars: " << chars << endl;
//
//						delete word_ptr;
//						word_ptr = NULL;
//						return word_ptr;
//					}
					if (!lparent) {
						string_array lca(ca);
						lca.pop_back();
						lparent = add(lca, lang, 0);
					}
					if (!rparent) {
						string_array rca(ca);
						rca.erase(rca.begin());
						rparent = add(rca, lang, 0);
					}

					word_ptr->lparent(lparent); word_ptr->lchar(find(lc));
					word_ptr->rparent(rparent);word_ptr->rchar(find(rc));

					assert(word_ptr->lparent() != NULL);
					assert(word_ptr->rparent() != NULL);
					assert(word_ptr->lchar() != NULL);
					assert(word_ptr->rchar() != NULL);
				}

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
		//	cerr << "skipping ..." << endl;
	}

	if (word_ptr->get_last_seen_document_id() != current_document_id_)
		word_ptr->increase_document_frequency();

	word_ptr->set_last_seen_document_id(current_document_id_);
	return word_ptr;
}

void Freq::add(word_ptr_type word_ptr, bool allnew) {
	//if (word_ptr->chars() == "ettoreximenes")
	//	cerr << " I got you " << endl;

	assert(word_ptr != NULL);
	if (word_ptr->size() > k_)
		k_ = word_ptr->size();

	if (!allnew)
		freq_.insert(make_pair(word_ptr->chars(), word_ptr));
	freq_n_[word_ptr->size()].push_back(word_ptr);
}

void Freq::sort(int k) {
	cerr << endl;
	cerr << "now sorting the results" << endl;

	if (freq_n_[k].size() > 0) {
		cerr << "total words: " << freq_n_[k].size() << endl;
		if (UNISEG_settings::instance().do_skip)
		    remove_low(k);
		cerr << "after skipping low: " << freq_n_[k].size() << endl;
		std::sort(freq_n_[k].begin(), freq_n_[k].end(), Word::cmp_freq);
	}

	cerr << "finished sorting" << endl;
}

void Freq::pile_up(int max)
{
	int i = k_;
	int size = 0;
	word_ptr_type next = NULL;
	word_ptr_type curr = NULL;
	word_ptr_type pre = NULL;

	for (int j = 0; j < freq_n_[i].size(); j++) {
		word_ptr_type lparent = freq_n_[i][j]; // current word pointer
		if ((j + 1) < freq_n_[i].size()
				&& lparent->next() != NULL
				&& lparent->next() == freq_n_[i][j + 1]) {

			word_ptr_type rparent = freq_n_[i][j + 1];
			word_ptr_type lchar = NULL;
			word_ptr_type rchar = NULL;

			string_array wa;
			string_array wa_next;

			wa.push_back(lparent->lchar()->chars());
			size = rparent->size();
			array_type next_wp_a;
			rparent->subarray(next_wp_a, 0, size);
			assert(next_wp_a.size() == size);
			for (int n = 0; n < size; n++) {
				wa.push_back(next_wp_a[n]->chars());
				wa_next.push_back(next_wp_a[n]->chars());
			}
//			cerr << "wa: ";
//			std::copy(wa.begin(), wa.end(), ostream_iterator<string_type>(cerr, " "));
//			cerr << endl;
			curr = add(wa, lparent->lang(), 0, true);
			//curr->lang(lparent->lang());

			lchar = (word_ptr_type)lparent->lchar();
			rchar = (word_ptr_type)rparent->rchar();
			curr->lparent(lparent);
			curr->lchar(lchar);
			curr->rparent(rparent);
			curr->rchar(rchar);
			assert(curr != NULL);

			if ((j + 2) < freq_n_[i].size()
					&& rparent->next() != NULL
					&& rparent->next() == freq_n_[i][j + 2]) {
				//std::copy(++wa.begin(), wa.end(), wa_next.begin());

				word_ptr_type rrparent = freq_n_[i][j + 2];
				wa_next.push_back(rrparent->rchar()->chars());
				next = add(wa_next, rparent->lang(), 0, true);
				assert(next != NULL);
				//next->lang(rparent->lang());

				lchar = (word_ptr_type)rparent->lchar();
				rchar = (word_ptr_type)rrparent->rchar();
				next->lparent(rparent);
				next->lchar(lchar);
				next->rparent(rrparent);
				next->rchar(rchar);

//				cerr << "wa_next: ";
//				std::copy(wa_next.begin(), wa_next.end(), ostream_iterator<string_type>(cerr, " "));
//				cerr << endl;
//				curr->next(next);
//				next->pre(curr);
//				pre = next;
			} // if
			curr->pre(pre);
			curr->next(next);
			if (next)
				next->pre(curr);
			if (pre)
				pre->next(curr);
			pre = next;
			curr = NULL;
			next = NULL;

			j++;
		} // if
	}
	if (k_ > i && k_ < max)
		pile_up(max);
}

void Freq::showcol(int n, int min, bool details) {
	cerr << endl;
	cerr << "total arrays #: " << k_ << endl;
	cerr << "listing words with size : " << n << endl;
	cerr << "number of words:" ;
	if (n <= k_) {
		cerr << freq_n_[n].size() << endl;
		array_type temp_arr;
		int i = 0;
		for (i = 0; i < (int)freq_n_[n].size(); i++)
			if (freq_n_[n][i]->freq() >= min)
				temp_arr.push_back(freq_n_[n][i]);
		std::sort(temp_arr.begin(), temp_arr.end(), Word::cmp_just_freq);

		for (i = 0; i < (int)temp_arr.size(); i++)
			//cerr << ->chars();
			temp_arr[i]->print(details);
	} else
		cerr << " 0" << endl;

}

void Freq::show(int n, int min, bool details) {
	for (int i = 1; i <= n && i <= k_; i++)
		showcol(i, min, details);
}

void Freq::show() {
	freq_type::const_iterator iter;
	for (iter=freq_.begin(); iter != freq_.end(); ++iter)
		cerr << iter->first << endl;
}

void Freq::alloc(int k) {
	assert(k > 0);

	cerr << "calculating address for words with size " << k << endl;

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

	cerr << "total freq: " << freq << endl;
}

void Freq::remove_low(int k) {
	if (k == 1)
		return; /// we want to keep all the frequency for word with only one character

	/* Not a good idea to remove those low frequency word in the following way
	for (int i = (freq_n_[k].size() - 1); i >= 0; i++) {
		if (freq_n_[k][i]->freq() <= UNISEG_settings::instance().to_skip())
			freq_n_[k].erase(freq_n_[k].begin() + i);
		else
			break;
	}
	*/
	array_type::iterator pos = std::remove_if(
			freq_n_[k].begin(), freq_n_[k].end(), NeedSkip(UNISEG_settings::instance().to_skip));
	freq_n_[k].erase(pos, freq_n_[k].end());
}

bool Freq::need_skip(word_ptr_type w_ptr) {
	//return w_ptr->freq() <= UNISEG_settings::instance().to_skip;
	return w_ptr->freq() < 2;
}

void Freq::array_to_array(array_type& wa, string_array& ca) {
	for (int i = 0; i < (int)ca.size(); i++) {
		word_ptr_type w_ptr = find(ca[i]);

		//for debug
		//cerr << "finding " << ca[i] << endl;
		assert(w_ptr != NULL);
		wa.push_back(w_ptr);
	}
}

void Freq::cal_sum() {
	int k = 0;
	while (k <= k_/*UNISEG_settings::MAX_CHARS*/) {
		cal_sum_k(k);
		k++;
	}
}

void Freq::cal_sum_k(int k) {
		int freq = 0;
		for (int i = 0; i < (int)freq_n_[k].size(); i++) {
			freq += freq_n_[k][i]->freq();
		}
		sum_n_[k] = freq;
}

void Freq::cal_avg() {
	int k = 0;
	while (k <= k_/*UNISEG_settings::MAX_CHARS*/) {
		if (freq_n_[k].size() > 0)
			avg_n_[k] = static_cast<float>(sum_n_[k])/freq_n_[k].size();
		k++;
	}

	//for debug
	k = 0;
	cerr << "average freq of each size of words" << endl;
	while (k <= k_) {
		if (freq_n_[k].size() > 0)
			cerr << k << ": " << avg_n_[k] << " (with sum "
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

		if (UNISEG_settings::instance().debug)
			cerr << "Assigning " << iter->second->chars()
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
					tmp_wp = freq.add(sa, c_wp->lang(), freqc);

					assert(tmp_wp != NULL);
					//tmp_wp->cal_p(this->sum_k(1));

					if (UNISEG_settings::instance().debug)
						cerr << "Found new string pattern for the 1 of N+1: " << tmp_wp->chars()
						<< "(" << tmp_wp->freq() << ")"
						<< endl;
				}
				else
					tmp_wp->freq(tmp_wp->freq() + freqc);

			}
		}
	}
}

void Freq::justify(unsigned int freq) {
	for (int i = 0; i < freq_n_[k_].size(); i++)
		freq_n_[k_][i]->adjust(freq);
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

	freq_type::const_iterator iter;
	for (iter=freq_.begin(); iter != freq_.end(); ++iter) {

		if (iter->second->size() <= 4)
			iter->second->cal_a();
	}
}

void Freq::cal_word_ngmi_a(double base) {
	int step = k_ < 4 ? k_ : 4;
	for (int i = 1; i <= step; i++)
		for (int j = 0; j < freq_n_[i].size(); j++)
			freq_n_[i][j]->cal_p(base);

	if (k_ >= 4)
		for (int j = 0; j < freq_n_[4].size(); j++)
			freq_n_[4][j]->cal_ngmi_a(2);
}

void Freq::show_p() {
	if (UNISEG_settings::instance().debug) {
		freq_type::const_iterator iter;
		for (iter=freq_.begin(); iter != freq_.end(); ++iter)
			cerr << iter->second->chars() << ": " << iter->second->p()
			<< "(" << iter->second->freq() << ")"
			<< endl;
	}

	if (UNISEG_settings::instance().debug) {
		cerr << endl << "The association scores:" << endl;
		freq_type::const_iterator iter;
		for (iter=freq_.begin(); iter != freq_.end(); ++iter)
			cerr << iter->second->chars() << ": " << iter->second->a()
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

void Freq::load(word_ptr_type word)
{
	int len = 1;
	if (word != NULL && (len = word->size()) <= freq_files_.size()) {
		assert(len > 0);
		freq_files_[len]->read_term(word);
	}
}

void Freq::load(int index)
{
	if (index > 1) {
		if (index > freq_files_.size())
			index = freq_files_.size();

		--index;
		freq_files_[index]->load();
	}
	else if (index == -1) {
		for (int i = 1; i < freq_files_.size(); i++)
			freq_files_[i]->load();
	}
}

void Freq::load_freq(std::string path, int n, bool force) {

	if (k_ > -1 && !force)
		return;

	/************************************
	 *  LOAD FREQUENCY TABLE IN MEMORY
	 ***********************************/
	cerr << "loading frequency files " << endl;

	//FreqLoader loader(freq_);

	if (path.length() <= 0 || !File::exist(path.c_str())) {
		cerr << "cann't find the frequency table path: " << path << endl;
		return;
		//exit(-1);
	}

	//loader.load(UNISEG_settings::instance().freqs_path, n);
	k_ = 1;
	//	int num = 0;
	cerr << "Loading files from " << path << endl;
	UNISEG_settings::instance().load = true;

	while (k_ <= n) {
		string name = stringify(k_);

		FreqFile *freq_file = new FreqFile(name, this);
		freq_file->path(path);

		if (!freq_file->exist()) {
			k_--;
			std::cerr << "No such file:" << freq_file->fullpathname() << std::endl;
			delete freq_file;
			break;
		}

		freq_file->wlen(k_);
		freq_file->read_in_memory();
		freq_files_.push_back(freq_file);

//		if (k_ > 1) {
//			/************************************
//			 * LOAD THE INDEX OF TERMS ONLY FIRST
//			 ************************************/
//			//freq_file->read_with_index();
//			IndexFile idxf(name);
//			idxf.path(UNISEG_settings::instance().freqs_path);
//			idxf.wlen(k_);
//			idxf.read(freq_);
//		}
//		else
		if (k_ == 1)
			freq_file->read();
		else
			freq_file->load_index();

		k_++;
	}

	cal_sum_n_avg();
	//k_ = loader.count();

	loaded_ = true;
}

void Freq::cal_word()
{
	freq_type::const_iterator iter;
	for (iter=freq_.begin(); iter != freq_.end(); ++iter) {
		iter->second->cal_idf(number_of_documents_);
		iter->second->cal_icf((double)sum_k(1)/2.5);
	}
}

void Freq::smooth()
{
//	int i, j;
//	for (i = k_; i > 1; --i)
//		for (int j = 0; j < freq_n_[i].size(); j++)
//			if (freq_n_[i][j]->freq() > 0)
//				freq_n_[i][j]->adjust(-freq_n_[i][j]->freq());

	extend(k_);

//	for (int i = k_; i > 1; --i)
//		smooth(i);
}

void Freq::smooth(int k)
{
//	int i, j;
//	for (i = k_; i > 1; --i)
		for (int j = 0; j < freq_n_[k].size(); j++)
			if (freq_n_[k][j]->freq() > 0)
				freq_n_[k][j]->adjust(-freq_n_[k][j]->freq());
}

void Freq::extend(int k)
{
	for (int i = 1; i < freq_n_[k].size(); ++i) {
		word_ptr_type first = freq_n_[k][i - 1];
		word_ptr_type second = freq_n_[k][i];

		if (first->freq() == second->freq() &&
				((first->lparent() == second->rparent() && first->lparent()->freq() == first->freq())
					|| (first->rparent() == second->lparent()) && first->rparent()->freq() == first->freq())) {
				string_array ca;
				if (first->lparent() == second->rparent()) {
					second->to_string_array(ca);
					ca.push_back(first->rchar()->chars());
				}
				else {
					first->to_string_array(ca);
					ca.push_back(second->rchar()->chars());
				}
				word_ptr_type new_word = add(ca, first->lang(), first->freq());
//				first->adjust_freq(-first->freq());
//				second->adjust_freq(-second->freq());
				new_word->df(first->df());
		}
	}

	if (freq_n_[k + 1].size() > 0) {
		sort(k + 1);
		extend(k + 1);
	}
}
