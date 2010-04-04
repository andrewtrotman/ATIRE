/*
	FOCUS_ARTICLE.H
	---------------
*/
#ifndef FOCUS_ARTICLE_H_
#define FOCUS_ARTICLE_H_

#include "focus.h"
#include "parser.h"

/*
	class ANT_FOCUS_ARTICLE
	-----------------------
	This is the identity focuser that simply return the whole article.  Amongst other uses, it
	can be used to measure the performance of article retrieval using focused retrieval metrics
*/
class ANT_focus_article : public ANT_focus
{
public:
	ANT_focus_article(ANT_focus_results_list *result_factory) : ANT_focus(result_factory) {}
	virtual ~ANT_focus_article() {}

	virtual ANT_focus_result *focus(unsigned char *document, long *results_length, long long docid = 0, char *document_name = NULL, ANT_search_engine_accumulator *document_accumulator = NULL);
} ;

#endif /* FOCUS_ARTICLE_H_ */
