/*
	ASSESSMENT_INEX.H
	-----------------
*/
#ifndef ASSESSMENT_INEX_H_
#define ASSESSMENT_INEX_H_

#include "assessment_TREC.h"

/*
	class ANT_ASSESSMENT_INEX
	-------------------------
*/
class ANT_assessment_INEX : public ANT_assessment_TREC
{
friend class ANT_assessment_factory;

protected:
	ANT_assessment_INEX() {}

public:
	ANT_assessment_INEX(ANT_memory *mem, char **docid_list, long long documents) : ANT_assessment_TREC(mem, docid_list, documents) {}
	ANT_relevant_document *read(char *filename, long long *judgements);
} ;



#endif  /* ASSESSMENT_INEX_H_ */
