/*
 * doc_seg.cpp
 *
 *  Created on: Nov 29, 2008
 *      Author: monfee
 */

#include "doc_seg.h"
#include "app_conf.h"
#include "qfreq.h"
#include "convert.h"
#include "udoc.h"
#include "qdoc.h"
#include "mdoc.h"
#include "fdoc.h"
#include "doc_freq.h"

#include <iostream>

using namespace std;

std::set<string_type>	DocSeger::seged_;

DocSeger::DocSeger(string name) :
	doc_(name, AppConf::instance().lang(), stream_)
	//, doc_freq_()
	//, doc_counter_(doc_freq_)
	 {

}

DocSeger::~DocSeger() {}

void DocSeger::start() {
	build();
	seg();
}

void DocSeger::output() {
	//cout << endl << stream_ << endl << endl;
	show_doc_freq();

}

void DocSeger::build() {
	/************************************
	 * EXTRACT THE TEXT FROM THE DOCUMENT
	 ************************************/
	doc_.parse();
}

void DocSeger::seg() {

	//longest_seg();
	//me_seg();
	if (AppConf::instance().howtoseg() == "qseg")
		q_seg();
	else if (AppConf::instance().howtoseg() == "sproat")
		mi_seg();
	else if (AppConf::instance().howtoseg() == "dai")
		mi2_seg();
	else {
		cout << "Please specify the way to do the segmentation. --way qseg|sproat|dai"
			<< endl;
	}
}

/**
 * from left to right, finding longest matched frequent string from table
 *
 */
void DocSeger::longest_seg() {
	/************************************
	 * 		  START SEGMENTATION
	 *finding the longest frequency string
	 ************************************/
	Freq& freq = QFreq::instance().freq();


/*	for (int i = 0; i < article_.length(); i++) {
		string& set = article_[i];

		do {d

		} while();
	}*/
	doc_.reset();
	while (doc_.more()) {
		//article_.push_back(string());

		iterator begin;
		iterator end;
		iterator cur;

		doc_.next(begin, end);
		cur = begin;

		word_ptr_type w_ptr = NULL;
		//cur++;

		while (cur < end) {
			string_type str = (*cur)->to_string();
			if (AppConf::instance().lang() == stpl::ENGLISH) {
				// for debug
				//cout << "before transform: " << str << endl;
				tolower(str);
				//cout << "after transform: " << str << endl;
			}

			w_ptr = freq.find(str);
			if (!w_ptr) {
				cerr << "There is no such character(" << (*cur)->to_string() << ") in frequency table" << endl;
				exit(1);
			}
			cur++;
			w_ptr = find_longest_freq_string(w_ptr, cur, end);

			cout << w_ptr->chars() << " ";
		}
		cout << endl;
	}
}

void DocSeger::me_seg() {
	cal_doc_freq();
	UDoc udoc(&doc_, &seged_);
	udoc.seg();
	//udoc.collect(seged_);
}

void DocSeger::mi_seg() {
	MDoc mdoc(&doc_);
	mdoc.seg();
}

void DocSeger::mi2_seg() {
	FDoc fdoc(&doc_);
	fdoc.seg();
}

void DocSeger::q_seg() {
	/************************************
	 *  doing the N + 1 frequency building
	 *
	 ************************************/
	cout << "Starting building frequency for the one of N+1" << endl;
	Freq a_freq;
	FreqCounter counter(&a_freq);
	counter.count(doc_, /*QConf::instance()->max()*/3, 1); //may be any value between 3~5,
														   //because we don't really want to get much noise
														   // 3 may be better, with chances to break the word
	a_freq.cal_sum();
	a_freq.add_freq(QFreq::instance().freq(), 1);
	cout << "Finished building frequency for the one of N+1" << endl;

	QDoc qdoc(&doc_, &seged_);
	qdoc.qzerit();
}

word_ptr_type DocSeger::find_longest_freq_string(const word_ptr_type w_ptr, iterator& begin, iterator end) {
	word_ptr_type l_ptr = w_ptr;

	while(begin <= end) {
		//cout << (*cur)->to_string();
		//article_[article_.length() - 1].append((*cur)->to_string());
		string_type str = (*begin)->to_string();
		if (AppConf::instance().lang() == stpl::ENGLISH) {
			// for debug
			//cout << "before transform: " << str << endl;
			tolower(str);
			//cout << "after transform: " << str << endl;
		}
		//word_ptr_type tmp_ptr = l_ptr->find_rchild((*begin)->to_string());
		word_ptr_type tmp_ptr = l_ptr->find_rchild(str);
		if(!tmp_ptr)
			break;

		l_ptr = tmp_ptr;

		if (begin == end)
			break;

		begin++;
	}
	return l_ptr;
}

void DocSeger::cal_doc_freq() {
	//doc_counter_.count(doc_, QFreq::instance().count(), 1);
	//doc_counter_.assign_array();
	//doc_counter_.remove_array_r();
	Freq freq;
	FreqCounter counter(&freq);
	counter.count(doc_, QFreq::instance().count(), 1);

	counter.assign_array();
	//counter.remove_array_r();
	if (AppConf::instance().verbose())
		counter.show_array();

	cout << endl << endl;
	counter.overall(QFreq::instance().freq());
	if (AppConf::instance().verbose())
		counter.show_array();
}

void DocSeger::show_doc_freq() {
	//doc_counter_.show_array();
}

bool DocSeger::find(string_type word) {
	return seged_.find(word) != seged_.end();
}

void DocSeger::insert(string_type word) {
	if (!find(word))
		seged_.insert(word);
}

void DocSeger::write() {
	cout << "Segmented words #: " << seged_.size() << endl;
	if (AppConf::instance().filename().size() > 0) {
		ofstream outfile (AppConf::instance().filename().c_str(),ofstream::binary);
		set<string_type>::iterator it = seged_.begin();
		for (; it != seged_.end(); ++it) {
			outfile << *it << endl;
		}

		outfile.close();
	}
}
