/*
	FOCUS_SENTENCE_TF.H
	-------------------
*/
#ifndef FOCUS_SENTENCE_TF_H_
#define FOCUS_SENTENCE_TF_H_

#include "focus.h"
#include "parser.h"

/*
	class ANT_FOCUS_SENTENCE_TF
	---------------------------
*/
class ANT_focus_sentence_tf : public ANT_focus
{
private:
	ANT_parser parser;

public:
	ANT_focus_sentnece_tf(ANT_focus_results_list *result_factory) : ANT_focus(result_factory) {}
	virtual ~ANT_focus_sentence_tf() {}

	virtual ANT_focus_result *focus(unsigned char *document, long *results_length, long long docid = 0, char *document_name = NULL, ANT_search_engine_accumulator *document_accumulator = NULL);
} ;



#endif /* FOCUS_SENTENCE_TF_H_ */
