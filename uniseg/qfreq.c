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

}

QFreq::~QFreq() {

}

void QFreq::load_freq(int n, bool force) {
	freq_stat_.load_freq(UNISEG_settings::instance().freqs_path, n, force);
	freq_training_.load_freq(UNISEG_settings::instance().training_path, n, force);
}

void QFreq::load(word_ptr_type word)
{
	freq_stat_.load(word);
	freq_training_.load(word);
}

Freq& QFreq::freq()
{
	/*if (!loaded_) load_freq(); */
	return freq_stat_;
}

word_ptr_type QFreq::find(string_type word) {
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

QFreq& QFreq::instance() {
	static QFreq inst;
	return inst;
}
