/*
	DIRECTORY_ITERATOR_MULTIPLE_INTERNALS.H
	---------------------------------------
*/
#ifndef DIRECTORY_ITERATOR_MULTIPLE_INTERNALS_H_
#define DIRECTORY_ITERATOR_MULTIPLE_INTERNALS_H_

#include "directory_iterator_object.h"
#include "threads.h"

class ANT_directort_iterator;
class ANT_directory_iterator_multiple;

/*
	class ANT_DIRECTORY_ITERATOR_MULTIPLE_INTERNALS
	-----------------------------------------------
*/
class ANT_directory_iterator_multiple_internals
{
public:
	ANT_directory_iterator *iterator;
	ANT_directory_iterator_multiple *parent;
	ANT_directory_iterator_object file_object;
	long get_file;
	ANT_thread_id_t thread_id;						// the OS thread ID
	long instance;									// which thread we are
	long long files_read;							// number of files read from this iterator;
} ;

#endif /* DIRECTORY_ITERATOR_MULTIPLE_INTERNALS_H_ */
