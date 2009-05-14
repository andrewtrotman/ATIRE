/*
	ASSESSMENT_INEX.H
	-----------------
*/
#ifndef __ASSESSMENT_INEX_H__
#define __ASSESSMENT_INEX_H__

#include "assessment.h"

/*
	class ANT_ASSESSMENT_INEX
	-------------------------
*/
class ANT_assessment_INEX : public ANT_assessment
{
friend class ANT_assessment_factory;

protected:
	ANT_assessment_INEX() {}

public:
	ANT_assessment_INEX(ANT_memory *mem, char **docid_list, long long documents) : ANT_assessment(mem, docid_list, documents) {}
	ANT_relevant_document *read(char *filename, long long *reldocs);
} ;



#endif __ASSESSMENT_INEX_H__
