/*
	NEXI_TERM_ITERATOR.H
	--------------------
*/
#ifndef __NEXI_TERM_ITERATOR_H__
#define __NEXI_TERM_ITERATOR_H__

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

#endif __NEXI_TERM_ITERATOR_H__
