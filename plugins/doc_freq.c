#include "doc_freq.h"
#include "qconf.h"
#include "doc.h"
#include "convert.h"

#include <cassert>
#include <algorithm>

//using namespace stpl;
using namespace std;
//using namespace UNICODE;

void FreqCounter::count(int max, int min) {
	//cout << "Text content: " << endl;
	//cout << "\"" << stream_ << "\"" << endl << endl;

	//stpl::UNICODE::DefaultChineseParser c_parser(stream_.begin(), stream_.end());
	//stpl::UNICODE::DefaultChineseParser::document_type& doc = c_parser.doc();

	//c_parser.parse();

	Doc a_doc(QConf::instance()->lang(), stream_);
	a_doc.parse();
	if (a_doc.doc().count() <= 0)
		return;

	count(a_doc, max, min);
}

void FreqCounter::count(Doc& a_doc, int max, int min) {

	/*entity_iterator prev = a_doc.doc().iter_begin();
	entity_iterator it = a_doc.doc().iter_begin();
	entity_iterator begin = a_doc.doc().iter_begin();*/

	a_doc.reset();
	while (a_doc.more()) {
		entity_iterator begin;
		entity_iterator end;

		a_doc.next(begin, end);
		int cmax = end - begin + 1;
		if ( max < cmax)
			cmax = max;

		add_word(begin, end, cmax, min);
	}

}

/// below is old style way to do the text frequency counting
/// now all the correct characters are extracted

/*	/// find the first valid character
	do {
		if ((*it)->lang() == QConf::instance()->lang()) {
			begin = it;
			it++;
			break;
		}
		it++;
	} while (it != a_doc.doc().iter_end());

	for (; it != a_doc.doc().iter_end(); it++) {
		if ((*it)->lang() != QConf::instance()->lang()) continue;

		// boundary arrives, like punctuation, symbols, characters in other languages
		if (it != a_doc.doc().iter_begin() && (*it)->begin() != (*prev)->end()) {
			//cout << endl;
			add_word(begin, prev, max, min);
			begin = it;
		}

		//cout << (*it)->to_string() << endl;
		prev = it;
	}

	// last setence
	if (begin != prev)
		add_word(begin, prev, max, min);
}*/

void FreqCounter::add_word(entity_iterator begin, entity_iterator end, int max, int min) {
	int len = end - begin + 1;
	entity_iterator from;
	entity_iterator to;

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
			from = begin + j;
			to = from + i;

			entity_iterator start = from;
			Freq::string_array ca;
			for (; start != to; start++) {
				string_type str = (*start)->to_string();
				if (QConf::instance()->lang() == stpl::ENGLISH) {
					// for debug
					//cout << "before transform: " << str << endl;
					tolower(str);
					//cout << "after transform: " << str << endl;
				}
				ca.push_back(str);
				//ca.push_back((*start)->to_string());
			}

			freq_->add(ca);
			//cout << string_type((*from)->begin(), (*to)->end()) << endl;
		}
	}
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
	std::sort(wa_.begin(), wa_.end(), SEGMENTATION::Word::cmp_just_freq);
	remove_array_r();

	/// we need to do the sort again, because after remove the redundancy
	/// the frequency changes
	std::sort(wa_.begin(), wa_.end(), SEGMENTATION::Word::cmp_just_freq);
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

