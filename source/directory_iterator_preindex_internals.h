/*
	DIRECTORY_ITERATOR_PREINDEX_INTERNALS.H
	---------------------------------------
*/
#ifndef DIRECTORY_ITERATOR_PREINDEX_INTERNALS_H_
#define DIRECTORY_ITERATOR_PREINDEX_INTERNALS_H_

class ANT_directory_iterator_preindex;
class ANT_parser;
class ANT_readability_factory;
class ANT_stem;

/*
	class ANT_DIRECTORY_ITERATOR_PREINDEX_INTERNALS
	-----------------------------------------------
	We have one of these per thread - its the thread local objects
*/
class ANT_directory_iterator_preindex_internals
{
public:
	ANT_directory_iterator_preindex *parent;
	ANT_parser *parser;
	ANT_readability_factory *readability;
	ANT_stem *stemmer;
	long segmentation;

public:
	ANT_directory_iterator_preindex_internals(ANT_directory_iterator_preindex *parent);
	virtual ~ANT_directory_iterator_preindex_internals();
} ;

#endif /* DIRECTORY_ITERATOR_PREINDEX_INTERNALS_H_ */
