/*
 * qfreq.h
 *
 *  Created on: Jan 15, 2009
 *      Author: monfee
 */

#ifndef QFREQ_H_
#define QFREQ_H_

#include "freq.h"
#include "dic.h"
#include <climits>

class FreqFile;

class QFreq {

private:
	Freq 						freq_stat_;
	Freq 						freq_training_;
	Freq						*freq_text_;  // the frequency information of to-be-segmented text

	Dic							dic_;

	double						base_;
	//int 						k_;

public:
	QFreq();
	~QFreq();

	void load_freq(int n = INT_MAX, bool force = false);
	void load_dic();

	Freq& freq();
	Freq& freq_stat() { return freq_stat_; }

	Freq& freq_training() { return freq_training_; }
	//int k() { return k_; }
	//int count() { return k_; }
	//bool is_loaded() { return loaded_; }
	void clear();
	void create_new_freq();
	Freq *freq_text() { return freq_text_; }

	void load(word_ptr_type word);
	word_ptr_type find(string_type& word);
	bool fuzzy_search_dic(string_type word);
	bool fuzzy_search_dic_right(string_type word);

	bool is_word(string_type& word);

	bool eligibility_check(word_ptr_type word);
	bool need_eligibility_check();
	bool is_possible_oov(string_type& word);

	double base() { return base_; }

	static QFreq& instance();
};

#endif /* QFREQ_H_ */
