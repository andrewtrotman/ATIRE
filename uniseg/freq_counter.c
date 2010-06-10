#include "freq_counter.h"
#include "uniseg_settings.h"
#include <ctype.h>

#include <cassert>
#include <list>
#include <algorithm>
#include <iostream>
#include <iterator>

using namespace std;

void FreqCounter::init()
{
	enc_ = UNISEG_encoding_factory::instance().get_encoding();
}

void FreqCounter::count(int max, int min)
{
	const char *start = (char *)stream_.c_str();
	const char *end = start + stream_.length();

	add_word(start, end, max, min);
	//count(start, end, max, min);
}

void FreqCounter::add_word(const char *begin, const char *end, int max, int min)
{
	int len = end - begin + 1;
	//char *from = NULL;
	//char *to = NULL;
	char *start = NULL;
	int count = 0;
	uniseg_encoding *enc = uniseg_encoding_factory::instance().get_encoding();

	if (len < min)
		return;

	/******************************************************
	 * TODO
	 * performance improvement method:
	 *     since if a word cannot be found in the table
	 *     that means any string contain that word should
	 *     be ignored
	 *
	 *     make a varable jstart = 0, and int j = jstart,
	 *      if a word is not added successful,
	 *      then jstart = j + i;
	 *
	 *****************************************************/


	// i - the number of characters depends on min
	// may from 1, 2, 3, 4, ... till less than max
	typedef list<string_type>	string_list;
	long pre_lang;

	for (int i = min; i >= 0 && i <= max; i++) {
	//cerr << "I got " << string_type((*begin)->begin(), (*end)->end()) << endl;
	//int i = max;
		//array_type	ca;
//		if ( i == max)
//			cerr << "Stop here , I need to see what you got" << endl;
		start = (char *)begin;
		//string_array last_ca;
		string_list cl; // chars list
		//for (int j = 0; j < (len - i) + 1; j++) {
		enc->test_char((unsigned char *)start);

		while (start < end) {
			if (cl.size() > 0)
				cl.pop_front();

			while ( start < end ) {
				//string_type str(start, 3);
				string_type str; // ((*start)->to_string();
				pre_lang = enc->lang();
				if (enc->lang() == uniseg_encoding::ALPHA
				    || enc->howmanybytes() > 1) {

					if (enc->lang() != uniseg_encoding::CHINESE && !UNISEG_settings::instance().split_latin_char)
					    while (start < end && enc->lang() != uniseg_encoding::CHINESE && !UNISEG_settings::instance().split_latin_char) {
						    // for debug
						    //cerr << "before transform: " << str << endl;
						    char c = *start;

						    tolower(c);
						    str.push_back(c);
						    //cerr << "after transform: " << str << endl;
						    start += enc->howmanybytes();
						    enc->test_char((unsigned char *)start);
						    if (enc->lang() == uniseg_encoding::NUMBER
						    		|| enc->lang() == uniseg_encoding::PUNCT
						    		|| enc->lang() == uniseg_encoding::SPACE)
							    break;
					    }
					else {
						str = string_type(start, enc->howmanybytes());
						start += enc->howmanybytes();
						enc->test_char((unsigned char *)start);
					}

				} else if (enc->lang() == uniseg_encoding::NUMBER) {
					while (start < end && enc->lang() == uniseg_encoding::NUMBER) {
						str.push_back(*start);
						start += enc->howmanybytes();
						enc->test_char((unsigned char *)start);
					}
				} else {
					start += enc->howmanybytes();
					enc->test_char((unsigned char *)start);
				}

				if ((pre_lang & languages_) && str.length() > 0) {
					cl.push_back(str);
					if (cl.size() == i)
						break;
				} else {
					if (cl.size() < i)
						cl.clear();
					break;
				}
				//ca.push_back((*start)->to_string());
			}

			if (cl.size() > 0) {
				string_array ca(cl.size());
				std::copy(cl.begin(), cl.end(), ca.begin());
				word_ptr_type ret_word = freq_->add(ca); //, pre_lang);
				assert(ret_word->freq() > 0);
			}
			//cerr << string_type((*from)->begin(), (*to)->end()) << endl;
		}
	}
}

void FreqCounter::auto_count()
{


}

void FreqCounter::count_segmented()
{
	uniseg_encoding *enc = uniseg_encoding_factory::instance().get_encoding();
	const char *start = (char *)stream_.c_str();
	//const char *end = start + stream_.length();
	word_ptr_type ww_ptr = NULL;
	word_ptr_type first_ptr = NULL;
	word_ptr_type second_ptr = NULL;
	string_array *ca1 = NULL;
	string_array *ca2 = NULL;

	while (*start != '\0') {
		string first;

		enc->test_char((unsigned char *)start);
		// skip the non
		while (*start != '\0' && enc->lang() != languages_) {
			start += enc->howmanybytes();
			enc->test_char((unsigned char *)start);
		}

		if (*start == '\0')
			break;

		// get the first word
		while (*start != '\0' && enc->lang() == languages_) {
			first.append(string_type(start, enc->howmanybytes()));
			start += enc->howmanybytes();
			enc->test_char((unsigned char *)start);
		}

		if (ca1 != NULL)
			delete ca1;

		ca1 = new string_array;
		to_string_array(first, *ca1);
		first_ptr = freq_->add(*ca1); //, languages_);
		freq_->add_to_array(first_ptr);
		first_ptr->is_word(true);

		while (*start != '\0') {
			string second;
			bool restart = false;

			// skip the non
			while (*start != '\0' && *start == ' ')
				++start;

			enc->test_char((unsigned char *)start);
			if (*start != '\0' && enc->lang() != languages_) {
				start += enc->howmanybytes();
				break;
			}

			// get the second
			// enc->test_char((unsigned char *)start);
			while (*start != '\0' && enc->lang() == languages_) {
				second.append(string_type(start, enc->howmanybytes()));
				start += enc->howmanybytes();
				enc->test_char((unsigned char *)start);
			}

			if (second.length() == 0)
				break;

			if (ca2 != NULL)
				delete ca2;
			ca2 = new string_array;
			to_string_array(second, *ca2);
			second_ptr = freq_->add(*ca2); //, languages_);
			freq_->add_to_array(second_ptr);
			second_ptr->is_word(true);

			//string word_pair = first + second;
			//string_array ca(ca1->size() + ca2->size());
			//std::merge(ca1->begin(), ca1->end(), ca2->begin(), ca2->end(), ca.begin());
			string_array ca(*ca1);
			//std::copy(ca2->begin(), ca2->end(), ca.end());
			for (int i = 0; i < ca2->size(); ++i)
				ca.push_back((*ca2)[i]);
			//to_string_array(word_pair, ca);
			word_ptr_type ww_ptr = freq_->add(ca); //, languages_);
			freq_->add_to_array(ww_ptr);
			ww_ptr->left(first_ptr);
			ww_ptr->right(second_ptr);
//			word_ptr_type w_ptr;
//
//			w_ptr = freq_->find(first);
//			assert(w_ptr != NULL);
//			if (!w_ptr->is_word())
//				w_ptr->is_word(true);
//			w_ptr->increase();
//			ww_ptr->lparent(w_ptr);
//
//			w_ptr = freq_->find(second);
//			assert(w_ptr != NULL);
//
//			if (!w_ptr->is_word())
//				w_ptr->is_word(true);
//			w_ptr->increase();

			first = second;
			first_ptr = second_ptr;
			second_ptr = NULL;
			delete ca1;
			ca1 = ca2;
			ca2 = NULL;
		}
	}

	if (ca1 != NULL)
		delete ca1;
	if (ca2 != NULL)
		delete ca2;
}

void FreqCounter::count_ones(Freq& freq, const char *begin, const char *end)
{
	char *start = NULL;
	int count = 0;
	uniseg_encoding *enc = uniseg_encoding_factory::instance().get_encoding();

	// i - the number of characters depends on min
	// may from 1, 2, 3, 4, ... till less than max
	typedef list<string_type>	string_list;


		start = (char *)begin;
		string_list cl; // chars list
		enc->test_char((unsigned char *)start);
		long pre_lang = enc->lang();

		word_ptr_type pre = NULL;
		word_ptr_type curr = NULL;

		while (start < end) {

				//string_type str(start, 3);
				string_type str; // ((*start)->to_string();
				bool increase_flag = false;

				if (enc->lang() == uniseg_encoding::ALPHA
				    || enc->howmanybytes() > 1) {

					if (enc->lang() != uniseg_encoding::CHINESE && !UNISEG_settings::instance().split_latin_char)
					    while (enc->lang() != uniseg_encoding::CHINESE && !UNISEG_settings::instance().split_latin_char && start < end) {
						    // for debug
						    //cerr << "before transform: " << str << endl;
						    char c = *start;

						    c = tolower(c);
						    str.push_back(c);
						    //cerr << "after transform: " << str << endl;
						    start += enc->howmanybytes();
						    enc->test_char((unsigned char *)start);
						    if (enc->lang() == uniseg_encoding::NUMBER
						    		|| enc->lang() == uniseg_encoding::PUNCT
						    		|| enc->lang() == uniseg_encoding::SPACE)
							    break;
					    }
					else {
						str = string_type(start, enc->howmanybytes());
						start += enc->howmanybytes();
						enc->test_char((unsigned char *)start);
					}

				} else if (enc->lang() == uniseg_encoding::NUMBER) {
					while (enc->lang() == uniseg_encoding::NUMBER && start < end) {
						str.push_back(*start);
						start += enc->howmanybytes();
						enc->test_char((unsigned char *)start);
					}
				} else
					increase_flag = true;

				if (str.length() > 0) {
					cl.push_back(str);

					string_array ca(cl.size());
					std::copy(cl.begin(), cl.end(), ca.begin());
					curr = freq.add(ca, pre_lang, true);
					//curr->lang(pre_lang);
//					std::copy(ca.begin(), ca.end(), ostream_iterator<string_type>(cerr, " "));
//					cerr << endl;
					cl.clear();

					if (pre != NULL && pre->lang() == curr->lang()) {
						pre->next(curr);
						curr->pre(pre);
					}
					pre = curr;
				} else {
					if (*start != ' '
							|| (pre != NULL
							&& pre->lang() != curr->lang()))
						pre = NULL;
				}

				if (increase_flag) {
					start += enc->howmanybytes();
					enc->test_char((unsigned char *)start);
				}
				pre_lang = enc->lang();
		}
}

void FreqCounter::count(const char *begin, const char *end, int max, int min)
{
	Freq freq;
	count_ones(freq, begin, end);
	freq.pile_up(max);
	freq.mergeto(*freq_);
}

void FreqCounter::show_array() {
	array_type::iterator it = wa_.begin();
	for (; it != wa_.end(); it++) {
		if ((*it)->seged()/* || (*it)->size() == 1*/)
			continue;
		if ( (*it)->freq() < 2)
			continue;
		cerr << (*it)->chars() << " : " << (*it)->freq() << endl;
	}

}

void FreqCounter::assign_array() {
	wa_.clear();
	//assert(wa_.size() == 0);
	freq_->to_array(wa_);
	std::sort(wa_.begin(), wa_.end(), Word::cmp_just_freq);
	remove_array_r();

	/// we need to do the sort again, because after remove the redundancy
	/// the frequency changes
	std::sort(wa_.begin(), wa_.end(), Word::cmp_just_freq);
}

void FreqCounter::remove_array_r() {
	array_type::iterator it = wa_.begin();
	for (; it != wa_.end(); it++) {
		if  ((*it)->seged())
			continue;

		if ((*it)->freq() > 1) {
			Freq a_freq;
			FreqCounter counter(&a_freq);
			counter.stream((*it)->chars());

			counter.count((*it)->size(), 1);
			a_freq.assign_freq(*freq_);
			a_freq.reduce_freq(*freq_, (*it));
			//a_freq.set_seged(freq_, (*it)->freq());
			(*it)->seged(false);
		}
	}
}

void FreqCounter::compat_array(array_type& wa) {
	array_type::iterator it = wa_.begin();
	for (; it != wa_.end(); it++) {
		if ((*it)->seged())
			continue;
		if ( (*it)->freq() < 2)
			continue;
		wa.push_back((*it));
	}
}

void FreqCounter::overall(Freq& freq) {
	freq_->assign_freq(freq);
	assign_array();
}
