/*
	DIRECTORY_ITERATOR_SPAM_FILTER.H
	--------------------------------
*/
#ifndef DIRECTORY_ITERATOR_SPAM_FILTER_H_
#define DIRECTORY_ITERATOR_SPAM_FILTER_H_

#include "directory_iterator.h"

/*
	class ANT_DIRECTORY_ITERATOR_SPAM_FILTER
	----------------------------------------
*/
class ANT_directory_iterator_spam_filter : public ANT_directory_iterator
{
public:
	enum { EXCLUDE = 0, INCLUDE };

private:
	ANT_directory_iterator *source;
	long long method;
	static char **docids;
	static long long number_docs;

	inline long should_index(char *docid);

public:
	ANT_directory_iterator_spam_filter(ANT_directory_iterator *source, char *filename, long long threshold, long long method, long get_file = 0);
	~ANT_directory_iterator_spam_filter();

	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object);
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);
} ;

#endif /* DIRECTORY_ITERATOR_SPAM_FILTER_H_ */
