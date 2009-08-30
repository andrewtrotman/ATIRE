/*
	NEXI_TERM_ITERATOR.C
	--------------------
*/
#include "NEXI_term_iterator.h"
#include "NEXI_term.h"

/*
	ANT_NEXI_TERM_ITERATOR::FIRST()
	-------------------------------
*/
ANT_NEXI_term *ANT_NEXI_term_iterator::first(ANT_NEXI_term *root)
{
start = current = root;

if (root == NULL)
	return NULL;

current = current->next;
return root;
}

/*
	ANT_NEXI_TERM_ITERATOR::NEXT()
	------------------------------
*/
ANT_NEXI_term *ANT_NEXI_term_iterator::next(void)
{
ANT_NEXI_term *answer;

if (current == NULL)
	return NULL;
answer = current;
current = current->next;
return answer;
}
