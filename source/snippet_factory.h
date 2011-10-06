/*
	SNIPPET_FACTORY.H
	-----------------
*/
#ifndef SNIPPET_FACTORY_H_
#define SNIPPET_FACTORY_H_

class ANT_snippet;
class ANT_stem;
class ANT_search_engine;

/*
	class ANT_SNIPPET_FACTORY
	-------------------------
*/
class ANT_snippet_factory
{
public:
	enum { SNIPPET_TITLE = 1, SNIPPET_BEGINNING, SNIPPET_TF, SNIPPET_TFICF, SNIPPET_BEST_TFICF_TAG, SNIPPET_BEST_TF_TAG, SNIPPET_WORD_CLOUD_KL };

public:
	ANT_snippet_factory() {}
	virtual ~ANT_snippet_factory() {}

	static ANT_snippet *get_snippet_maker(long type, long length_of_snippet, long length_of_longest_document, char *tag, ANT_search_engine *engine = NULL, ANT_stem *stemmer = NULL, long cloud_terms = 10);
} ;

#endif /* SNIPPET_FACTORY_H_ */
