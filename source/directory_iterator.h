/*
	DIRECTORY_ITERATOR.H
	--------------------
*/
#ifndef DIRECTORY_ITERATOR_H_
#define DIRECTORY_ITERATOR_H_

#include "disk.h"

/*
	class ANT_DIRECTORY_ITERATOR
	----------------------------
*/
class ANT_directory_iterator : public ANT_disk
{
public:
	ANT_directory_iterator() {};
	virtual ~ANT_directory_iterator() {};

	virtual char *first(char *wildcard) { return ANT_disk::get_first_filename(wildcard); }
	virtual char *next(void) { return ANT_disk::get_next_filename(); }
} ;

#endif  /* DIRECTORY_ITERATOR_H_ */

