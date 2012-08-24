/*
	DIRECTORY_ITERATOR_MIME_FILTER.H
	--------------------------------
*/
#ifndef DIRECTORY_ITERATOR_MIME_FILTER_H_
#define DIRECTORY_ITERATOR_MIME_FILTER_H_

#include "directory_iterator.h"

/*
	class ANT_DIRECTORY_ITERATOR_MIME_FILTER
	----------------------------------------
*/
class ANT_directory_iterator_mime_filter : public ANT_directory_iterator
{
private:
	ANT_directory_iterator *source;

public:
	ANT_directory_iterator_mime_filter(ANT_directory_iterator *source, long get_file = 0);
	virtual ~ANT_directory_iterator_mime_filter();

	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object);
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);
} ;

#endif /* DIRECTORY_ITERATOR_MIME_FILTER_H_ */
