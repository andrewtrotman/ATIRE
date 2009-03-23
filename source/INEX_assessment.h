/*
	INEX_ASSESSMENT.H
	-----------------
*/

#ifndef __INEX_ASSESSMENT_H__
#define __INEX_ASSESSMENT_H__

class ANT_relevant_document;
class ANT_memory;

class ANT_INEX_assessment
{
private:
	long *numeric_docid_list;
	long **sorted_numeric_docid_list;
	ANT_memory *memory;
	size_t documents;

private:
	char *max(char *a, char *b, char *c);
	static int cmp(const void *a, const void *b);

public:
	ANT_INEX_assessment(ANT_memory *mem, char **docid_list, size_t documents);
	ANT_relevant_document *read(char *filename, size_t *reldocs);
} ;



#endif __INEX_ASSESSMENT_H__
