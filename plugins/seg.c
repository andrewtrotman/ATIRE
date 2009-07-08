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

Seger::Seger(Freq& allfreq, word_ptr_type tw_ptr) :
	allfreq_(allfreq), tw_ptr_(tw_ptr), stream_(tw_ptr_->chars()) {}

Seger::Seger(Freq&	allfreq, const string_type stream) : allfreq_(allfreq), stream_(stream) {
	tw_ptr_ = allfreq_.find(stream);
}

Seger::Seger(Freq&	allfreq, const char* stream, size_t length) : allfreq_(allfreq), stream_(stream, length) {

}


Seger::~Seger() {}

void Seger::start() {
	build();
	//apply_rules();
	seg();


	//add_to_list(cwlist_);
	//mark_the_seged();
}

void Seger::output() {

}

void Seger::build() {
	assert (stream_.length() > 0);
	//assert(tw_ptr_->chars().length() > 0);
	FreqCounter counter(stream_, &freq_);
	counter.count();
	assign_freq();
	do_some_calculations();
	//justify(0);

	bool stop = false;
	word_ptr_type local_tw_ptr = freq_.find(stream_);
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
		CWords* temp = new CWords(size, w_ptr);
		clist_.insert(temp);
		delete temp;
		w_ptr = (word_ptr_type)w_ptr->lparent();
	} while(w_ptr != NULL);
	clist_.apply_rules();

	while (!stop) {
		stop = true;
		cwords_list::iterator it = clist_.list().begin();
		cwords_list::iterator prev;
		while( it != clist_.list().end() ) {
			int n = 0; /// n copies of it

			if ((!((*it)->is_end()))/* || (*it)->reach_limit()*/) {
				/// for debug
				//cout << "it size: " << (*it)->words().size() << endl;
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
				//it = clist_.list().erase(it);
				it = clist_.delete_node(it);
				//delete cws_ptr;
				clist_.append(clist);
				//apply_rules();
				//it = clist_.list().begin();
			}
			else
				++it;
			//if (!stop)
			//	clist_.listopithecus().erase(prev);
		}
	}

	// remove those nodes without end
	// clist_.remove_no_end();
	if (UNISEG_settings::instance().verbose) {
		TIMINGS_DECL();
		TIMINGS_START();

		cout << "finished building all the possible combinations for :"
			<< endl << "\""	<< local_tw_ptr->chars() << "\"("
			<< local_tw_ptr->size()<< ")" << endl
			<< "with total possibilities: " << clist_.size()
			<< endl;

		TIMINGS_END("build");
	}
}

void Seger::make(CList& clist, string_type& str) {
	word_ptr_type w_ptr = freq_.find(str);

	//cwords_list::reverse_iterator it = clist_.list().rbegin();
	//cout << "clist size: " << clist.list().size() << endl;
	cwords_list::reverse_iterator it = clist.list().rbegin();
	while(it != clist.list().rend()) {
		//cout << "1" << endl;
		assert (w_ptr != NULL);
		(*it)->append(w_ptr);
		w_ptr = (word_ptr_type)w_ptr->lparent();

		++it;
	}
	assert(w_ptr == NULL);
}

void Seger::assign_freq() {
	freq_.assign_freq(allfreq_);
}

void Seger::justify(unsigned int min) {
	freq_.justify(min);
}

void Seger::do_some_calculations() {
	freq_.cal_word_p(QFreq::instance().freq().sum_k(1));

	freq_.cal_word_a();

	freq_.show_p();
}
/**
 * the freq here is different with the freq_, the local freq_ is used to
 * stored the possible combinations of segmentations
 */
void Seger::seg() {
	clist_.cal(&freq_);

	if (UNISEG_settings::instance().mean == 4
			|| UNISEG_settings::instance().mean == 5)
		clist_.sort(false);
	else
		clist_.sort(true);
}

void Seger::show_all() {
	clist_.show();
}

void Seger::add_to_list(array_type& cwlist) {

	if (clist_.size() < 1)
		return;

	assert(clist_.size() > 0);

	do {
		array_type temp = clist_.front()->to_array();
		clist_.list().pop_front();
		assert(temp.size() > 0);

		if (temp.size() > 1) {
			string str("");

			for (int i = 0; i < temp.size(); i++) {
				str.append(temp[i]->chars());
				word_ptr_type w_ptr = allfreq_.find(temp[i]->chars());

				if (!w_ptr)
					continue;

				w_ptr->is_word(true);
				cwlist.push_back(w_ptr);

				if (i > 0 && i < (temp.size() -1)) {
					word_ptr_type iner_w_ptr = allfreq_.find(str);
					// for debug
					//cout << "setting " << str << " for being a word or seged" << endl;
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
	} while (clist_.size() > 0);

	if (tw_ptr_ != NULL)
		tw_ptr_->seged(true);
	//w_ptr = tw_ptr_;
	//mark_the_seged(tw_ptr_);
}

void Seger::mark_the_seged() {
	/// to set some other words for being seged as well
	if (tw_ptr_ != NULL) {
		const unsigned int freq = tw_ptr_->freq();
		freq_.set_seged(allfreq_, freq);
	}
}

