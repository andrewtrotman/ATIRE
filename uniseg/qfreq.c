/*
 * qfreq.cpp
 *
 *  Created on: Jan 15, 2009
 *      Author: monfee
 */

#include "qfreq.h"
#include "uniseg_settings.h"
#include "freq_loader.h"
#include "convert.h"
#include "freq_file.h"
#include "index_file.h"

#include <iostream>

using namespace std;


QFreq::QFreq() {
	k_ = -1;
	loaded_ = false;
}

QFreq::~QFreq() {

}

void QFreq::load_freq(int n, bool force) {

	if (k_ > -1 && !force)
		return;

	/************************************
	 *  LOAD FREQUENCY TABLE IN MEMORY
	 ***********************************/
	cerr << "loading frequency files " << endl;

	//FreqLoader loader(freq_);

	if (UNISEG_settings::instance().freqs_path.length() <= 0) {
		cerr << "cann't find the frequency table path " << endl;
		exit(-1);
	}

	//loader.load(UNISEG_settings::instance().freqs_path, n);
	k_ = 1;
	//	int num = 0;
	cerr << "Loading files from " << UNISEG_settings::instance().freqs_path << endl;
	UNISEG_settings::instance().load = true;

	while (k_ <= n) {
		string name = stringify(k_);

		FreqFile freq_file(name, freq_);
		freq_file.path(UNISEG_settings::instance().freqs_path);
		freq_file.wlen(k_);

		if (!freq_file.exist()) {
			k_--;
			std::cerr << "No such file:" << freq_file.fullpathname() << std::endl;
			break;
		}

		if (k_ > 1) {
			/************************************
			 * LOAD THE INDEX OF TERMS ONLY FIRST
			 ************************************/
			//freq_file.read_with_index();
			IndexFile idxf(name);
			idxf.path(UNISEG_settings::instance().freqs_path);
			idxf.wlen(k_);
			idxf.read(freq_);
		}
		else
			freq_file.read();

		k_++;
	}

	freq_.cal_sum_n_avg();
	//k_ = loader.count();

	loaded_ = true;
}

QFreq& QFreq::instance() {
	static QFreq inst;
	return inst;
}
