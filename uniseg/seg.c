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
}

void Seger::init()
{
	free();

	assert(stream_.length() > 0);

	clist_ = new CList;

	//assert(tw_ptr_->chars().length() > 0);
	freq_ = new Freq;
	FreqCounter counter(stream_, freq_);
	counter.count(UNISEG_settings::instance().max, 1);

	if (!allfreq_)
		allfreq_ = &(QFreq::instance().freq());

	if (!tw_ptr_)
		tw_ptr_ = allfreq_->find(stream_);

	assign_freq();
	do_some_calculations();
	//justify(0);
}

void Seger::input(unsigned char *input, int length)
{
	stream_ = string_type((char *)input, length);
	init();
}

void Seger::load_frqs()
{
	QFreq::instance().load_freq();
}

void Seger::start()
{
	build();
	//apply_rules();
	seg();


	add_to_list(words_list_);
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

//unsigned char** Seger::output()
//{
//	if (!output_ && words_list_.size() > 0) {
//		output_ = new unsigned char *[words_list_.size() + 1];
//		output_[words_list_.size()] = NULL;
//
//		for (int i = 0; i < words_list_.size(); i++) {
//			//cerr << words_list_[i]->chars() << endl;
//			int len = words_list_[i]->chars().length();
//			output_[i] = new unsigned char[len];
//			for (int k = 0; k < len; k++)
//				output_[i][k] = words_list_[i]->chars()[k];
//			//strncpy((char *)output_[i], words_list_[i]->chars().c_str(), len);
//			cerr << output_[i] << endl;
//			//output_[i][len] = '\0';
//		}
//	}
//	return output_;
//}
//
//int Seger::output(unsigned char** out)
//{
//	if (!out && words_list_.size() > 0) {
//		out = new unsigned char *[words_list_.size() + 1];
//		out[words_list_.size()] = NULL;
//
//		for (int i = 0; i < words_list_.size(); i++) {
//			//cerr << words_list_[i]->chars() << endl;
//			int len = words_list_[i]->chars().length();
//			out[i] = new unsigned char[len + 1];
//			for (int k = 0; k < len; k++)
//				out[i][k] = words_list_[i]->chars()[k];
//			unsigned char *word = out[i];
//			word[len] = '\0';
//			//strncpy((char *)out[i], words_list_[i]->chars().c_str(), len);
//			cerr << word << endl;
//			//out[i][len] = '\0';
//		}
//	}
//	return words_list_.size();
//}
//
//void Seger::free_output()
//{
//	int i = 0;
//	while (output_[i]) {
//		delete output_[i];
//		i++;
//	}
//	delete [] output_;
//	output_ = NULL;
//}

void Seger::build()
{
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
	freq_type& freq = freq_->set();
	freq_type::const_iterator iter;
	for (iter = freq.begin(); iter != freq.end(); ++iter) {
		word_ptr_type local_word  = iter->second;
		word_ptr_type global_word = allfreq_->find(local_word->chars());
		if (global_word)
			QFreq::instance().load(global_word);
		word_pairs.insert(make_pair(local_word, global_word));
	}

	std::map<word_ptr_type, word_ptr_type>::iterator p_iter;
	for (p_iter = word_pairs.begin(); p_iter != word_pairs.end(); ++p_iter) {
		word_ptr_type local_word  = p_iter->first;
		word_ptr_type global_word = p_iter->second;
		if (!global_word || global_word->freq() <= 0)
			local_word->freq(1);
		else
			local_word->freq(global_word->freq());
	}
}

void Seger::justify(unsigned int min) {
	freq_->justify(min);
}

void Seger::do_some_calculations() {
	freq_->cal_word_p(QFreq::instance().freq().sum_k(1));

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
	CWords *second = clist_->second();

	int stop_word_count1 = first->chinese_stop_word_count();
	int stop_word_count2 = second == NULL ? -1 : second->chinese_stop_word_count();

	CWords *best = (stop_word_count2 > stop_word_count1) ? second : first;
	do {
		array_type temp = best->to_array();
		clist_->list().pop_front();
		assert(temp.size() > 0);

		if (temp.size() > 1) {
			string str("");

			for (int i = 0; i < temp.size(); i++) {
				str.append(temp[i]->chars());
				word_ptr_type w_ptr = allfreq_->find(temp[i]->chars());

				if (!w_ptr)
					continue;

				w_ptr->is_word(true);
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
				tw_ptr_->is_word(true);
				cwlist.push_back(tw_ptr_);
				break;
			}
		}
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

