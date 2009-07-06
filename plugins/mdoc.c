/*
 * mdoc.cpp
 *
 *  Created on: May 14, 2009
 *      Author: monfee
 */

#include "mdoc.h"
#include "uniseg_settings.h"
#include <limits>
#include <cmath>

using namespace std;

MPair::MPair(word_ptr_type w_ptr, int pos): w_(w_ptr), pos_(pos) {
	score_ = std::numeric_limits<double>::min();
//	if (w_ptr != NULL)
//		score_ = w_ptr->a();
}

MDoc::MDoc(Doc* doc)  : DocBase::DocBase(doc) {
	init();
}

MDoc::~MDoc() {

}

void MDoc::init() {
	freq_->cal_word_p(QFreq::instance().freq().sum_k(1));
	//freq_->cal_word_a();
}

void MDoc::seg_sentence(entity_iterator& begin, entity_iterator& end) {
	create_pairs(begin, end);
	cal_assc_strength();
	if (UNISEQ_settings::instance().verbose())
		show_assc_strength();
	//pairs_.sort(MPair::cmp);
	pair<int, int> unseged = get_first_unfil(0);

	while(unseged.first != UNAVAILABlE/* && (unseged.second - unseged.first) > 0*/) {
		extract_bigram(unseged.first, unseged.second);
		unseged = get_first_unfil(0);
	}
}

void MDoc::create_pairs(entity_iterator& begin, entity_iterator& end) {
	if (pairs_.size()) {
		for (int i = 0; i < pairs_.size(); i++)
			delete pairs_[i];
		pairs_.clear();
	}

	entity_iterator first = begin;
	entity_iterator second = first;
	second++;
	while (second <= end) {
		string bigram = (*first)->to_string() + (*second)->to_string();
		MPair* pair =  new MPair(freq_->find(bigram), first - begin);
		pair->bigram(bigram);
		pairs_.push_back(pair);
		first = second;
		second++;
	}
}

void MDoc::extract_bigram(int start, int end) {
	MPair* pair = pairs_[start];
	double a = pair->score();

	MPair* next = NULL;

	for (int i = start + 1; i < end && i < pairs_.size(); i++) {
		next = pairs_[i];
		if (next->score() > a) {
			a = next->score();
			pair = next;
		}
	}


	if (a > 2.5) {
		fill(pair->pos(), 2);
		cout << "Found word: " << pair->word()->chars() << endl;
	}
	else {
		for (int i = start; i <= end; i++)
			fill(i, 1);
	}
}


void MDoc::cal_assc_strength() {
	for (int i = 0; i < pairs_.size(); i++) {
		MPair* pair = pairs_[i];
		word_ptr_type w = pair->word();
		double score = -std::numeric_limits<double>::max();
		if (w != NULL) {
			word_ptr_type c1 = w->lparent();
			word_ptr_type c2 = w->rparent();
			assert(c1 != NULL);
			assert(c2 != NULL);

			score = log2(w->p() / (c1->p()*c2->p()));
		}
		pair->score(score);
	}
}

void MDoc::show_assc_strength() {
	for (int i = 0; i < pairs_.size(); i++) {
		cout << "building pair for bigram " << pairs_[i]->bigram();
		cout << "(" << pairs_[i]->score() << ")"<< endl;
	}
}
