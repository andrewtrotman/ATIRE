/*
	DIRECTORY_ITERATOR_MULTIPLE.H
	-----------------------------
*/
#ifndef DIRECTORY_ITERATOR_MULTIPLE_H_
#define DIRECTORY_ITERATOR_MULTIPLE_H_

#include "directory_iterator.h"

/*
	class ANT_DIRECTORY_ITERATOR_MULTIPLE
	-------------------------------------
*/
class ANT_directory_iterator_multiple : public ANT_directory_iterator
{
protected:
	ANT_directory_iterator **sources;
	long sources_used, sources_length, current_source;
	char **filename;
	char **file;
	long long *length;

public:
	ANT_directory_iterator_multiple();
	virtual ~ANT_directory_iterator_multiple();

	void add_iterator(ANT_directory_iterator *iterator);

	virtual char *first(char *wildcard);
	virtual char *next(void);
	virtual char *read_entire_file(long long *length = 0);
} ;


#endif /* DIRECTORY_ITERATOR_MULTIPLE_H_ */
