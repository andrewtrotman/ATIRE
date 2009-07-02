#include "qzer.h"
#include "app_conf.h"
#include "freq_loader.h"
#include "doc_freq.h"
#include "qtypes.h"
#include "convert.h"
#include "qfreq.h"

#include <algorithm>
#include <iostream>
#include <iterator>

using namespace std;

QZer::QZer() : freq_(QFreq::instance().freq()){
	init();
}

QZer::~QZer() {
	cwlist_.clear();
}

void QZer::init() {
	/*cout << "loading frequency files " << endl;

	FreqLoader loader(freq_);

	loader.load(AppConf::instance().wd());
	freq_.cal_sum_n_avg();
	k_ = loader.count();*/
}

void QZer::start() {
	//Freq freq;
	if (AppConf::instance().phrase().length() > 0) {
		doit(AppConf::instance().phrase());
	}
	else {
		cout << endl << "Starting segmentation" << endl;
		cout << "total arrays: " << k_ << endl;

		int k = k_ - 1; //freq_.array_size() - 1;
		//while ( k > 3) {
			Freq::array_type& wa = freq_.array_k(k);
			cout << endl << "segmenting size: " << wa.size() << endl;

			/// that is a list of phrase candidates that needed to be segmented
			/// or it is just a single word

			for (int i = 0; i < wa.size(); i++) {
			//	cout << "segmenting " << wa[i]->chars() << endl;
				//Seger seger(wa[i]->chars());
			//string temp("gardenofepicurus");
				if (wa[i]->is_word() || wa[i]->seged())
					continue;

				doit(wa[i]);
			}

			--k;
		//}
	}
}

void QZer::doit(const word_ptr_type w_ptr) {
	Seger seger(freq_, w_ptr);
	do_others(seger);
}

void QZer::doit(const string_type& phrase) {
	Seger seger(freq_, phrase);
	do_others(seger);
}

void QZer::do_others(Seger& seger) {
	seger.build();
	//seger.apply_rules();
	seger.seg();

	if (AppConf::instance().verbose())
		seger.show_all();

	seger.add_to_list(cwlist_);
	seger.mark_the_seged();
}

void QZer::show() {
	for (int i = 0; i < cwlist_.size(); i++)
		cout << cwlist_[i]->chars() << " ";
	cout << endl;
}

void QZer::save() {
	if (AppConf::instance().save_good()) {
		FreqLoader saver(freq_);
		saver.save(AppConf::instance().good());
	}
}
