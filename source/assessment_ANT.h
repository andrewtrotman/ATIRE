/*
	ASSESSMENT_ANT.H
	----------------
*/
#ifndef __ASSESSMENT_ANT_H__
#define __ASSESSMENT_ANT_H__

#include "assessment.h"

/*
	class ANT_ASSESSMENT_ANT
	------------------------
*/
class ANT_assessment_ANT : public ANT_assessment
{
friend class ANT_assessment_factory;

protected:
	ANT_assessment_ANT() {}

public:
	ANT_assessment_ANT(ANT_memory *mem, char **docid_list, long long documents) : ANT_assessment(mem, docid_list, documents) {}
	virtual ANT_relevant_document *read(char *filename, long long *reldocs);
} ;


#endif __ASSESSMENT_ANT_H__
