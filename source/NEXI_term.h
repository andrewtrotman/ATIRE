/*
	NEXI_TERM.H
	-----------
*/
#ifndef NEXI_TERM_H_
#define NEXI_TERM_H_

#include "string_pair.h"

/*
	class ANT_NEXI_TERM
	-------------------
	This requires some explination
*/
class ANT_NEXI_term
{
public:
	ANT_NEXI_term *next, *parent_path;
	ANT_string_pair path;
	ANT_string_pair term;
	long sign;				// -ve, 0 or +ve (for +/- terms)
	double tf_weight;							// PRESCALAR:the tf value used in the ranking function is (tf * tf_weight)
	double rsv_weight;							// POSTSCALAR: the value added to the accumulators is rsv_weight * rank(tf)
	double query_frequency;						// the number of times the term occurs in the query (BM25's k3)

public:
	void text_render(void);
	ANT_string_pair *get_term(void) { return &term; }
	long get_sign(void) { return sign; }		// returns 0 for "normal" terms or +ve or -ve for "weighted" terms
} ;

#endif  /* NEXI_TERM_H_ */

