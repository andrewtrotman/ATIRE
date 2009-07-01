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

class QFreq {

private:
	Freq freq_;
	int k_;

public:
	QFreq();
	~QFreq();

	void load_freq(int n = INT_MAX, bool force = false);
	Freq& freq() { return freq_; }
	int k() { return k_; }
	int count() { return k_; }

	static QFreq& instance();
};

#endif /* QFREQ_H_ */
