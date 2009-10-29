/*
	NEXI_TERM_ANT.C
	---------------
*/

#include "NEXI_term_ant.h"

/*
	ANT_NEXI_TERM_ANT::CMP_COLLECTION_FREQUENCY()
	---------------------------------------------
*/
int ANT_NEXI_term_ant::cmp_collection_frequency(const void *a, const void *b)
{
ANT_NEXI_term_ant *one, *two;
one = *(ANT_NEXI_term_ant **)a;
two = *(ANT_NEXI_term_ant **)b;

return one->term_details.collection_frequency < two->term_details.collection_frequency ? -1 : one->term_details.collection_frequency == two->term_details.collection_frequency ? 0 : 1;
}

/*
	ANT_NEXI_TERM_ANT::CMP_DOCUMENT_FREQUENCY()
	-------------------------------------------
*/
int ANT_NEXI_term_ant::cmp_document_frequency(const void *a, const void *b)
{
ANT_NEXI_term_ant *one, *two;
one = *(ANT_NEXI_term_ant **)a;
two = *(ANT_NEXI_term_ant **)b;

return one->term_details.document_frequency < two->term_details.document_frequency ? -1 : one->term_details.document_frequency == two->term_details.document_frequency ? 0 : 1;
}
