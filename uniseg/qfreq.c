/*
 * qfreq.cpp
 *
 *  Created on: Jan 15, 2009
 *      Author: monfee
 */

#include "qfreq.h"
#include "uniseg_settings.h"
#include "freq_loader.h"
#include "freq_file.h"
#include "index_file.h"

#include <iostream>

using namespace std;


QFreq::QFreq() {
	dic_.load(UNISEG_settings::instance().dics_path);
}

QFreq::~QFreq() {

}

void QFreq::load_freq(int n, bool force) {
	freq_stat_.load_freq(UNISEG_settings::instance().freqs_path, n, force);
	freq_training_.load_freq(UNISEG_settings::instance().training_path, n, force);

	if (freq_training_.freq_files().size() > 0)
		UNISEG_settings::instance().with_training_info = true;
}

void QFreq::load(word_ptr_type word)
{
	freq_stat_.load(word);
	freq_training_.load(word);
}

Freq& QFreq::freq()
{
	/*if (!loaded_) load_freq(); */
	if (freq_stat_.array_size() > 0)
		return freq_stat_;
	return freq_training_;
}

bool QFreq::is_word(string_type& word)
{
	bool ret = dic_.find(word);
	return ret;
}

word_ptr_type QFreq::find(string_type& word) {
	string_array ca;
	to_string_array(word, ca);

	int min = 1, max = ca.size(); //freq_stat_.array_size();
	//int len = ca.size();

	for (int i = min; i <= max; i++) {
		for (int j = 0; j < (max - i) + 1; j++) {
			//tmp_str.append(ca[j]);
			string_type tmp_str("");
			int k = j;
			while (k < (j + i)) {
				tmp_str.append(ca[k]);
				++k;
			}

			word_ptr_type tmp_word = freq_stat_.find(tmp_str);
			if (tmp_word != NULL)
				freq_stat_.load(tmp_word);
			//word_ptr_type tmp_word = freq_stat_.find();
		}
	}
	return freq_stat_.find(word);
}

bool QFreq::fuzzy_search_dic(string_type word)
{
	return dic_.fuzzy_search(word);
}

bool QFreq::need_eligibility_check()
{
	if (freq_training_.array_size() > 1) {
		return true;
	}
	return false;
}

bool QFreq::eligibility_check(word_ptr_type word)
{
	string word_str = word->chars();
	if (freq_training_.array_size() > 0) {
		word_ptr_type w_ptr = freq_training_.find(word_str);

		if (UNISEG_settings::instance().oov_check && freq_stat_.array_size() > 0) {
			if (!w_ptr || !w_ptr->is_word())
				return is_possible_oov(word_str);
			return true;
		}
		return !w_ptr ? false : w_ptr->is_word();
	}
	return true;
}

inline bool QFreq::is_possible_oov(string_type& word)
{
	return false;
}

QFreq& QFreq::instance() {
	static QFreq inst;
	return inst;
}
