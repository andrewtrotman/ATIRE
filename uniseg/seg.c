/*
 * SEG.CPP
 * -------
 *
 *  Created on: Nov 29, 2008
 *      Author: monfee
 */

#include "seg.h"
#include "clist.h"
#include "uniseg_settings.h"
#include "qfreq.h"
#include "freq_counter.h"

#include <cassert>
#include <cmath>

#include <iterator>
#define TIME_BUILD
#include "timings.h"

using namespace std;

Seger::Seger()
{
	init_members();
}

Seger::Seger(word_ptr_type tw_ptr) : stream_(tw_ptr_->chars())
{
	init_members();
	tw_ptr_ = tw_ptr;
	init();
}

Seger::Seger(const string_type stream) : stream_(stream)
{
	init_members();
	init();
}

Seger::Seger(const char* stream, size_t length) : stream_(stream, length)
{
	init_members();
	init();
}

Seger::~Seger()
{
	//free_output();
	free();
}

void Seger::free()
{
	if (words_list_.size() > 0)
		words_list_.clear();

	if (freq_) {
		delete freq_;
		freq_ = NULL;

		tw_ptr_ = NULL;
		tw_ptr_local_ = NULL;
	}

	if (clist_) {
	    delete clist_;
	    clist_ = NULL;
	}

}

void Seger::init_members()
{
	clist_ = NULL;
	freq_ = NULL;
	allfreq_ = NULL;
	tw_ptr_ = NULL;
	tw_ptr_local_ = NULL;
}

void Seger::init()
{
	free();

	assert(stream_.length() > 0);

	//assert(tw_ptr_->chars().length() > 0);
	freq_ = new Freq;
	FreqCounter counter(stream_, freq_);
	counter.count(UNISEG_settings::instance().max, 1);
	tw_ptr_local_ = freq_->array_k(freq_->array_size())[0];

	if (!allfreq_) {
		allfreq_ = &(QFreq::instance().freq());
		base_ = allfreq_->sum_k(1);
		double p = 1.0 / base_;
		UNISEG_settings::instance().threshold = log(p);
	}

	tw_ptr_ = allfreq_->find(stream_);
	assign_freq(tw_ptr_local_, tw_ptr_);

	assign_freq();
	do_some_calculations();

	//justify(0);
}

pair<word_ptr_type, word_ptr_type> Seger::get_leftmost_word(word_ptr_type word)
{
	word_ptr_type pre = NULL, cur, final = NULL;
	int len = 1;
	while ( len <= word->size() && ((cur = word->subword(0, len))->is_word() || QFreq::instance().fuzzy_search_dic(cur->chars()))) {
		pre = cur;
		++len;
	}

	if (cur != NULL && cur->size() > 2 /*&& cur->has_word_pair()*/) {
		pair<word_ptr_type, word_ptr_type> a_pair = break_tie(cur);
		final = a_pair.first;
	}
	else if (pre != NULL)
		final = pre;

	if (final != NULL) {
		word_ptr_type right = (final->size() == word->size()) ? (word_ptr_type)NULL : word->subword(final->size(), word->size() - final->size());
		return make_pair(final, right);
	}
	return make_pair(word->lchar(), word->rparent());
}

bool Seger::check_word_pair(word_ptr_type word)
{
	bool flag = false;
	int size = word->size();
	if (size > 3 && word->left() == NULL && word->right() == NULL) {
		word_ptr_type left = word->subword(0, size/2);
		word_ptr_type right = word->subword(size/2, size - size/2);
		//if ((left->is_word() || QFreq::instance().fuzzy_search_dic(left->chars())) && (right->is_word() || QFreq::instance().fuzzy_search_dic(right->chars()))) {
		if (left->is_word() && right->is_word()) {
			word->left(left);
			word->right(right);
			flag = true;
		}
		else {
			if ((size % 2) == 1) {
				left = word->subword(0, size/2 + 1);
				right = word->subword(size/2 + 1, size - size/2 - 1);
				//if ((left->is_word() || QFreq::instance().fuzzy_search_dic(left->chars())) && (right->is_word() || QFreq::instance().fuzzy_search_dic(right->chars()))) {
				if (left->is_word() && right->is_word()) {
					word->left(left);
					word->right(right);
					flag = true;
				}
			}
		}
	}
	return flag; //word->has_word_pair();
}

std::pair<word_ptr_type, word_ptr_type> Seger::break_tie(word_ptr_type word)
{
	pair<word_ptr_type, word_ptr_type> a_pair;
	if (check_word_pair(word))
		a_pair = make_pair(word->left(), word->right());
	else {
		if (word->rparent()->is_word() || QFreq::instance().fuzzy_search_dic(word->rparent()->chars())) {
			if (word->lparent()->is_word()) { // all parents are words
				if (word->lparent()->freq() < word->rparent()->freq())
					a_pair = make_pair(word->lchar(), word->rparent());
				else
					a_pair = make_pair(word->lparent(), word->rchar());
			}
			else
				a_pair = make_pair(word->lchar(), word->rparent());
		}
		else {
			if (word->lparent()->is_word() || QFreq::instance().fuzzy_search_dic(word->lparent()->chars()))
				a_pair = make_pair(word->lparent(), word->rchar());
			else
				a_pair = make_pair(word->lchar(), word->rparent());
		}
	}
	return a_pair;
}

void Seger::input(unsigned char *input, int length)
{
	stream_ = string_type((char *)input, length);
	init();
}

void Seger::input(string_type& stream)
{
	stream_ = stream;
	init();
}

void Seger::start()
{
	if (tw_ptr_local_->size() < 4 && QFreq::instance().freq_training().array_size() > 0) {
		if (tw_ptr_local_->size() == 3) {
			if (tw_ptr_local_->lparent()->is_word()) {
				if (tw_ptr_local_->rparent()->is_word()) { // all parents are words
					if (tw_ptr_local_->rparent()->freq() < tw_ptr_local_->lparent()->freq()) {
						words_list_.push_back(tw_ptr_local_->lparent());
						words_list_.push_back(tw_ptr_local_->rchar());
					}
					else {
						words_list_.push_back(tw_ptr_local_->lchar());
						words_list_.push_back(tw_ptr_local_->rparent());
					}
				}
				else {
					words_list_.push_back(tw_ptr_local_->lparent());
					words_list_.push_back(tw_ptr_local_->rchar());
				}
				return;
			}
			else {
				if (tw_ptr_local_->rparent()->is_word()) {
					words_list_.push_back(tw_ptr_local_->lchar());
					words_list_.push_back(tw_ptr_local_->rparent());
					return;
				}
			}
		} else {
			if (tw_ptr_local_->is_word()) {
				words_list_.push_back(tw_ptr_local_);
				return;
			}
		}
		words_list_.resize(tw_ptr_local_->array().size(), word_ptr_type(NULL));
		std::copy(tw_ptr_local_->array().begin(), tw_ptr_local_->array().end(), words_list_.begin());
	}
	else {
		build();
		//apply_rules();
		seg();

		add_to_list(words_list_);
	}
	//mark_the_seged();
}

const unsigned char *Seger::output()
{
	if (stream_out_.size() == 0) {
		for (int i = 0; i < words_list_.size(); i++)
			stream_out_.append(words_list_[i]->chars() + " ");
	}
	return (unsigned char *)stream_out_.c_str();
}

void Seger::build()
{
	clist_ = new CList;
	bool stop = false;
	word_ptr_type local_tw_ptr = freq_->find(stream_);
	word_ptr_type w_ptr = local_tw_ptr;
	//word_ptr_type r_ptr = (word_ptr_type)local_tw_ptr->rparent();
	assert (w_ptr != NULL);

	/**
	 * make sure the list is in following order:
	 * _
	 * __
	 * ___
	 * ____
	 * _____
	 * ______
	 * ...
	 *
	 * so the list has to be insert before the "begin" pos
	 */
	int size = local_tw_ptr->size();
	do {
		if (w_ptr->freq() > UNISEG_settings::instance().to_skip) {
		    CWords* temp = new CWords(size, w_ptr);
		    clist_->insert(temp);
		    delete temp;
		}
		w_ptr = (word_ptr_type)w_ptr->lparent();
	} while(w_ptr != NULL);
	clist_->apply_rules();

	while (!stop) {
		stop = true;
		cwords_list::iterator it = clist_->list().begin();
		cwords_list::iterator prev;
		while( it != clist_->list().end() ) {
			int n = 0; /// n copies of it

			if ((!((*it)->is_end()))/* || (*it)->reach_limit()*/) {
				/// for debug
				//cerr << "it size: " << (*it)->words().size() << endl;
				/// need n copies of it including the current one
				int pos = (*it)->pos();
				n = size - pos;
				CList clist(n, *it);
				//clist.show();
				assert(n == clist.list().size());
				string_type str = local_tw_ptr->subchars(local_tw_ptr->size() - n, n);
				make(clist, str);
				clist.apply_rules();
				//prev = it;
				stop = false;
				//CWords* cws_ptr;
				//cws_ptr = (*it);
				//it = clist_->list().erase(it);
				it = clist_->delete_node(it);
				//delete cws_ptr;
				clist_->append(clist);
				//apply_rules();
				//it = clist_->list().begin();
			}
			else
				++it;
			//if (!stop)
			//	clist_->listopithecus().erase(prev);
		}
	}

	// remove those nodes without end
	// clist_->remove_no_end();
	if (UNISEG_settings::instance().debug) {
		TIMINGS_DECL();
		TIMINGS_START();

		cerr << "finished building all the possible combinations for :"
			<< endl << "\""	<< local_tw_ptr->chars() << "\"("
			<< local_tw_ptr->size()<< ")" << endl
			<< "with total possibilities: " << clist_->size()
			<< endl;

		TIMINGS_END("build");
	}
}

void Seger::make(CList& clist, string_type& str) {
	word_ptr_type w_ptr = freq_->find(str);



	//cwords_list::reverse_iterator it = clist_->list().rbegin();
	//cerr << "clist size: " << clist.list().size() << endl;
	cwords_list::reverse_iterator it = clist.list().rbegin();
	while(it != clist.list().rend()) {
		//cerr << "1" << endl;
		assert (w_ptr != NULL);
		if (w_ptr->freq() > UNISEG_settings::instance().to_skip) {
		    (*it)->append(w_ptr);
		    ++it;
		}
		else {
		    cwords_list::iterator temp_it = clist.delete_node(--it.base());
		    it = cwords_list::reverse_iterator(temp_it);
		}

		w_ptr = (word_ptr_type)w_ptr->lparent();
	}
	assert(w_ptr == NULL);
}

void Seger::assign_freq() {
	//freq_->assign_freq(*allfreq_);
	std::map<word_ptr_type, word_ptr_type> word_pairs;
	std::map<word_ptr_type, word_ptr_type>::iterator it;
	freq_type& freq = freq_->set();

	int step = freq_->array_size() < allfreq_->array_size() ? freq_->array_size() : allfreq_->array_size();

	// need to load the frequency first, and during the loading the frequency could be changed
	for (int i = 1; i < step; ++i) {
		array_type& word_array = freq_->array_k(i);
		for (int j = 0; j < word_array.size(); ++j) {
			word_ptr_type local_word  = word_array[j];
			word_ptr_type global_word = allfreq_->find(local_word->chars());
			if (global_word)
				QFreq::instance().load(global_word);
			word_pairs.insert(make_pair(local_word, global_word));
		}
	}

	for (it = word_pairs.begin(); it != word_pairs.end(); ++it)
		assign_freq(it->first, it->second);
}

void Seger::assign_freq(word_ptr_type local_word, word_ptr_type global_word)
{
	if (global_word) {
		local_word->freq(global_word->freq());
		local_word->is_word(global_word->is_word());
		if (global_word->left() != NULL && global_word->left()->is_word()) {
			word_ptr_type tmp = freq_->find(global_word->left()->chars());
			local_word->left(tmp);
			local_word->left()->is_word(true);
		}
		if (global_word->right() != NULL && global_word->right()->is_word()) {
			local_word->right(freq_->find(global_word->right()->chars()));
			local_word->right()->is_word(true);
		}
	}
	else
		local_word->freq(0);
}

void Seger::justify(unsigned int min) {
	freq_->justify(min);
}

void Seger::do_some_calculations() {
	freq_->cal_word_p(base_);

	freq_->cal_word_a();

	freq_->show_p();
}
/**
 * the freq here is different with the freq_, the local freq_ is used to
 * stored the possible combinations of segmentations
 */
void Seger::seg() {
	clist_->cal(freq_);

	if (UNISEG_settings::instance().mean == 4
			|| UNISEG_settings::instance().mean == 5)
		clist_->sort(false);
	else
		clist_->sort(true);

	if (UNISEG_settings::instance().debug)
		show_all();
}

void Seger::show_all() {
	clist_->show();
}

void Seger::add_to_list(array_type& cwlist) {

	if (clist_->size() < 1)
		return;

	assert(clist_->size() > 0);

	CWords *first = clist_->front();
	CWords *best = first;

	if (UNISEG_settings::instance().stop_word_check && clist_->size() > 1) {
		int stop_word_count1 = first->chinese_stop_word_count();

		CWords *second = clist_->second();
		int stop_word_count2 = /*second == NULL ? -1 : */second->chinese_stop_word_count();

		if ((stop_word_count2 > stop_word_count1)) {
			clist_->list().pop_front();
			best =  second;
		}
	}
	do {
		array_type temp = best->to_array();
		assert(temp.size() > 0);

		if (temp.size() > 1) {
			string str("");

			for (int i = 0; i < temp.size(); i++) {
				str.append(temp[i]->chars());
				word_ptr_type w_ptr = allfreq_->find(temp[i]->chars());

				if (!w_ptr)
					continue;

				//w_ptr->is_word(true);
				cwlist.push_back(w_ptr);

				if (i > 0 && i < (temp.size() -1)) {
					word_ptr_type iner_w_ptr = allfreq_->find(str);
					// for debug
					//cerr << "setting " << str << " for being a word or seged" << endl;
					//assert(iner_w_ptr != NULL);
					if (iner_w_ptr != NULL)
						iner_w_ptr->seged(true);
				}
			}
			break;
		}
		else {
			if (tw_ptr_ != NULL) {
				//tw_ptr_->is_word(true);
				cwlist.push_back(tw_ptr_);
				break;
			}
		}
		clist_->list().pop_front();
		best = clist_->front();
	} while (clist_->size() > 0);

	if (tw_ptr_ != NULL)
		tw_ptr_->seged(true);
	//w_ptr = tw_ptr_;
	//mark_the_seged(tw_ptr_);
}

void Seger::mark_the_seged() {
	/// to set some other words for being seged as well
	if (tw_ptr_ != NULL) {
		const unsigned int freq = tw_ptr_->freq();
		freq_->set_seged(*allfreq_, freq);
	}
}

