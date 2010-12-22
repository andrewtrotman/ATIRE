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

public:
	void text_render(void);
	ANT_string_pair *get_term(void) { return &term; }
	long get_sign(void) { return sign; }		// returns 0 for "normal" terms or +ve or -ve for "weighted" terms
} ;

#endif  /* NEXI_TERM_H_ */

