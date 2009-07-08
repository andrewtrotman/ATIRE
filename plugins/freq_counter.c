#include "freq_counter.h"
#include "uniseg_settings.h"
#include "convert.h"

#include <cassert>
#include <algorithm>

using namespace std;

void FreqCounter::count(int max, int min)
{
const char *start = (char *)stream_.c_str();
const char *end = start + stream_.length();

add_word(start, end, max, min);
}

void FreqCounter::add_word(const char *begin, const char *end, int max, int min)
{
	int len = end - begin + 1;
	char *from;
	char *to;
	uniseg_encoding *enc = uniseg_encoding_factory::gen_encoding_scheme((uniseg_encoding_factory::encoding)UNISEG_settings::instance().encoding_scheme);

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
	for (int i = min; i >= 0 && i <= max; i++) {
	//cout << "I got " << string_type((*begin)->begin(), (*end)->end()) << endl;
	//int i = max;
		//array_type	ca;
		//if ( i == max)
		//	cout << "Stop here , I need to see what you got" << endl;

		for (int j = 0; j < (len - i) + 1; j++) {
			from = (char *)begin + j;
			to = from + i;

			char *start = from;
			string_array ca;
			for (; start != to;) {
				// TODO
				enc->test_char((unsigned char *)start);
				string_type str(start, enc->howmanybytes()); // ((*start)->to_string();
				if (enc->lang() == uniseg_encoding::ENGLISH) {
					// for debug
					//cout << "before transform: " << str << endl;
					tolower(str);
					//cout << "after transform: " << str << endl;
				}
				ca.push_back(str);
				start += enc->howmanybytes();
				//ca.push_back((*start)->to_string());
			}

			freq_->add(ca);
			//cout << string_type((*from)->begin(), (*to)->end()) << endl;
		}
	}
	delete enc;
}

void FreqCounter::show_array() {
	array_type::iterator it = wa_.begin();
	for (; it != wa_.end(); it++) {
		if ((*it)->seged()/* || (*it)->size() == 1*/)
			continue;
		if ( (*it)->freq() < 2)
			continue;
		cout << (*it)->chars() << " : " << (*it)->freq() << endl;
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

