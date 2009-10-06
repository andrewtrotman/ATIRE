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
private:
	char *current_filename;

public:
	ANT_directory_iterator() {};
	virtual ~ANT_directory_iterator() {};

	virtual char *first(char *wildcard) { return current_filename = ANT_disk::get_first_filename(wildcard); }
	virtual char *next(void) { return current_filename = ANT_disk::get_next_filename(); }
	using ANT_disk::read_entire_file;
	virtual char *read_entire_file(long long *len = 0) { return ANT_disk::read_entire_file(current_filename, len); }
} ;

#endif  /* DIRECTORY_ITERATOR_H_ */

