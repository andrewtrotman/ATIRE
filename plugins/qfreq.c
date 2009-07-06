/*
 * qfreq.cpp
 *
 *  Created on: Jan 15, 2009
 *      Author: monfee
 */

#include "qfreq.h"
#include "uniseg_settings.h"
#include "freq_loader.h"

#include <iostream>

using namespace std;


QFreq::QFreq() {
	k_ = -1;
}

QFreq::~QFreq() {

}

void QFreq::load_freq(int n, bool force) {

	if (k_ > -1 && !force)
		return;

	/************************************
	 *  LOAD FREQUENCY TABLE IN MEMORY
	 ***********************************/
	cout << "loading frequency files " << endl;

	FreqLoader loader(freq_);

	loader.load(UNISEQ_settings::instance().wd(), n);
	freq_.cal_sum_n_avg();
	k_ = loader.count();
}

QFreq& QFreq::instance() {
	static QFreq inst;
	return inst;
}
