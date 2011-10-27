/*
	SNIPPET_WORD_CLOUD.H
	--------------------
*/
#ifndef SNIPPET_WORD_CLOUD_H_
#define SNIPPET_WORD_CLOUD_H_

#include "snippet.h"

class ANT_relevance_feedback;

/*
	class ANT_SNIPPET_WORD_CLOUD
	----------------------------
*/
class ANT_snippet_word_cloud : public ANT_snippet
{
private:
	ANT_relevance_feedback *cloud_maker;
	long terms_to_generate;

public:
	ANT_snippet_word_cloud(unsigned long max_length, long length_of_longest_document, ANT_search_engine *engine, ANT_stem *stemmer, long terms);
	virtual ~ANT_snippet_word_cloud();

	virtual char *get_snippet(char *snippet, char *document);
	virtual ANT_NEXI_term_ant **parse_query(char *query) { (void)query; return NULL; }
} ;

#endif /* SNIPPET_WORD_CLOUD_H_ */
