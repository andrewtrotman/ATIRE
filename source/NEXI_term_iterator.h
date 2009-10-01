/*
	NEXI_TERM_ITERATOR.H
	--------------------
*/
#ifndef NEXI_TERM_ITERATOR_H_
#define NEXI_TERM_ITERATOR_H_

#include "string_pair.h"

class ANT_NEXI_term;

/*
	class ANT_NEXI_TERM_ITERATOR
	----------------------------
*/
class ANT_NEXI_term_iterator
{
private:
	ANT_NEXI_term *start, *current;

public:
	ANT_NEXI_term *first(ANT_NEXI_term *root);
	ANT_NEXI_term *next(void);
} ;

#endif  /* NEXI_TERM_ITERATOR_H_ */
