/*
	ASSESSMENT_TREC.H
	-----------------
*/
#ifndef ASSESSMENT_TREC_H_
#define ASSESSMENT_TREC_H_

#include "assessment.h"

/*
	class ANT_ASSESSMENT_TREC
	-------------------------
*/
class ANT_assessment_TREC : public ANT_assessment
{
friend class ANT_assessment_factory;

protected:
	ANT_assessment_TREC() {}

public:
	ANT_assessment_TREC(ANT_memory *mem, char **docid_list, long long documents) : ANT_assessment(mem, docid_list, documents) {}
	ANT_relevant_document *read(char *filename, long long *judgements);
} ;

#endif /* ASSESSMENT_TREC_H_ */
