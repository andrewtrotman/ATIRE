/*
 * qfreq.h
 *
 *  Created on: Jan 15, 2009
 *      Author: monfee
 */

#ifndef QFREQ_H_
#define QFREQ_H_

#include "freq.h"
#include <climits>
#include <vector>

class FreqFile;

class QFreq {

private:
	Freq 						freq_;
	int 						k_;
	bool 						loaded_;
	std::vector<FreqFile *> 	freq_files_;

public:
	QFreq();
	~QFreq();

	void load_freq(int n = INT_MAX, bool force = false);
	Freq& freq() { if (!loaded_) load_freq(); return freq_; }
	Freq& freq_noload() { return freq_; }
	int k() { return k_; }
	int count() { return k_; }
	bool is_loaded() { return loaded_; }

	word_ptr_type find(string_type word);
	void load(word_ptr_type word);
	void load(int index = -1);

	static QFreq& instance();
};

#endif /* QFREQ_H_ */
