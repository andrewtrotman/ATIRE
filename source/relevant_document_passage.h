/*
	RELEVANT_DOCUMENT_PASSAGE.H
	---------------------------
*/
#ifndef RELEVANT_DOCUMENT_PASSAGE_H_
#define RELEVANT_DOCUMENT_PASSAGE_H_

/*
	class ANT_RELEVANT_DOCUMENT_PASSAGE
	-----------------------------------
*/
class ANT_relevant_document_passage
{
public:
	long long offset, length;
	ANT_relevant_document_passage *next;			// next passage in this document (or NULL)
} ;


#endif /* RELEVANT_DOCUMENT_PASSAGE_H_ */
