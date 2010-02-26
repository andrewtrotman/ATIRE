/*
	RELEVANT_DOCUMENT.H
	-------------------
*/

#ifndef RELEVANT_DOCUMENT_H_
#define RELEVANT_DOCUMENT_H_

class ANT_relevant_document_passage;
/*
	class ANT_RELEVANT_DOCUMENT
	---------------------------
*/
class ANT_relevant_document
{
public:
	long topic;
	long long docid;
	long relevant_characters;
	long document_length;
	ANT_relevant_document_passage *passage_list;
	long number_of_relevant_passages;
	long best_entry_point;

public:
	static int compare(const void *a, const void *b);
} ;

#endif  /* RELEVANT_DOCUMENT_H_ */
