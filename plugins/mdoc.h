/*
 * mdoc.h
 *
 *  Created on: May 14, 2009
 *      Author: monfee
 */

#ifndef MDOC_H_
#define MDOC_H_

#include "doc_base.h"
#include "uniseg_types.h"

#include <string>

/**
 * mutual information pair
 */
class MPair {

private:
	word_ptr_type	w_;
	string_type		bigram_;
	int				pos_;
	double			score_; /// association score

public:
	MPair(word_ptr_type w_ptr, int pos);
	~MPair() {}

	const word_ptr_type word() const { return w_; }
	double score() { return score_; }
	void score(double score) { score_ = score; }
	int pos() { return pos_; }
	int end() { return pos_ + 1; }
	const string_type& bigram() const { return bigram_; }
	void bigram(string_type bigram) { bigram_ = bigram; }

//	static bool cmp(MPair& p1, MPair& p2) {
//		cmp(&p1, &p2);
//	}

	static bool cmp(MPair* p1, MPair* p2) {
		double s1 = p1->score();
		double s2 = p2->score();

		if (s1 == s2) return p1->pos() > p2->pos();
		return  s1 > s2;
	}
};

/**
 * This is a class for realizing the mutual information with its original formula by Sproat
 *
 */
class MDoc : public DocBase {
public:
	typedef std::vector<MPair*>										mpair_list;

protected:
	mpair_list														pairs_;

public:
	MDoc(Doc* doc);
	~MDoc();

	void seg_sentence(entity_iterator& begin, entity_iterator& end);
	virtual void cal_assc_strength();

private:
	void init();
	void create_pairs(entity_iterator& begin, entity_iterator& end);
	virtual void extract_bigram(int start, int end);
	void show_assc_strength();
};

#endif /* MDOC_H_ */
