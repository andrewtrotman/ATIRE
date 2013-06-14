/*
	DIRECTORY_ITERATOR_SPAM_FILTER.H
	--------------------------------
*/
#ifndef DIRECTORY_ITERATOR_SPAM_FILTER_H_
#define DIRECTORY_ITERATOR_SPAM_FILTER_H_

#include "directory_iterator_filter.h"

/*
	class ANT_DIRECTORY_ITERATOR_SPAM_FILTER
	----------------------------------------
*/
class ANT_directory_iterator_spam_filter : public ANT_directory_iterator_filter
{
public:
	ANT_directory_iterator_spam_filter(ANT_directory_iterator *source, char *filename, long long threshold, long get_file = 0);
} ;

#endif /* DIRECTORY_ITERATOR_SPAM_FILTER_H_ */
