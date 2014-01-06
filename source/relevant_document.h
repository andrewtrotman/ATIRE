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
#ifdef FILENAME_INDEX
	char *docid;
#else
	long long docid;
#endif
	long long subtopic;
	long long topic;
	
	long long document_length;
	long long relevant_characters; // also serves as judgement level
	
	// INEX-specific stuff
	ANT_relevant_document_passage *passage_list;
	long long number_of_relevant_passages;
	long long best_entry_point;

public:
	static int compare(const void *a, const void *b);
} ;

#endif  /* RELEVANT_DOCUMENT_H_ */
