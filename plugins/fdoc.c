/*
 * fdoc.cpp
 *
 *  Created on: May 14, 2009
 *      Author: monfee
 */

#include "fdoc.h"
#include "uniseg_settings.h"
#include <limits>

FDoc::FDoc(Doc* doc) : MDoc::MDoc(doc){

}

FDoc::~FDoc() {

}

void FDoc::extract_bigram(int start, int end){
	MPair* pair = pairs_[start];
	double a = pair->score();
	double threhold = UNISEQ_settings::instance().threshold();

	if (a > threhold) {
		MPair* next = NULL;

		int i = start + 1;
		if (i < end && i < pairs_.size()) {
			next = pairs_[i];
			if (next->score() > a) {
				fill(pair->pos(), 1);
				pair = next;
			}
		}

		fill(pair->pos(), 2);
	}
	else
		fill(start, 1);

}

void FDoc::cal_assc_strength() {
	for (int i = 0; i < pairs_.size(); i++) {
		MPair* pair = pairs_[i];
		word_ptr_type w = pair->word();
		double score = -std::numeric_limits<double>::max();
		if (w != NULL) {
			word_ptr_type c1 = w->lparent();
			word_ptr_type c2 = w->rparent();
			assert(c1 != NULL);
			assert(c2 != NULL);

			score = 0.39 * log2(w->p()) - 0.28 * log2(c1->p()) - 0.23 * log2(c2->p()) - 0.32;
		}
		pair->score(score);
	}
}
