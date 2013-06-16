/*
	DIRECTORY_ITERATOR_FILTER_SPAM.H
	--------------------------------
*/
#ifndef DIRECTORY_ITERATOR_FILTER_SPAM_H_
#define DIRECTORY_ITERATOR_FILTER_SPAM_H_

#include "directory_iterator_filter.h"

/*
	class ANT_DIRECTORY_ITERATOR_FILTER_SPAM
	----------------------------------------
*/
class ANT_directory_iterator_filter_spam : public ANT_directory_iterator_filter
{
public:
	ANT_directory_iterator_filter_spam(ANT_directory_iterator *source, char *filename, long long threshold, long get_file = 0);
} ;

#endif /* DIRECTORY_ITERATOR_FILTER_SPAM_H_ */
