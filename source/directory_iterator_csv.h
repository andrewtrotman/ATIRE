/*
	DIRECTORY_ITERATOR_CSV.H
	------------------------
	Documents are one per line in a single file
*/
#ifndef DIRECTORY_ITERATOR_CSV_H_
#define DIRECTORY_ITERATOR_CSV_H_

#include "directory_iterator_file.h"

/*
	class ANT_DIRECTORY_ITERATOR_CSV
	--------------------------------
*/
class ANT_directory_iterator_csv : public ANT_directory_iterator_file
{
public:
	ANT_directory_iterator_csv(char *file, long get_file = 0) : ANT_directory_iterator_file(file, get_file) {}
	virtual ~ANT_directory_iterator_csv() {}

	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);
} ;

#endif /* DIRECTORY_ITERATOR_CSV_H_ */
