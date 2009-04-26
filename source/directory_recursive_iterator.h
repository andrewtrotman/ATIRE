/*
	DIRECTORY_RECURSIVE_ITERATOR.H
	------------------------------
*/
#ifndef __DIRECTORY_RECURSIVE_ITERATOR_H__
#define __DIRECTORY_RECURSIVE_ITERATOR_H__

#include "directory_iterator.h"

/*
	class ANT_DIRECTORY_RECURSIVE_ITERATOR
	--------------------------------------
*/
class ANT_directory_recursive_iterator : protected ANT_directory_iterator
{
private:
	ANT_disk_directory *handle_stack;
	ANT_disk_directory *file_list;
	char *wildcard;

private:
	/*
		These routines are used for the recursive_get_next_filename routines
	*/
	long push_directory(void);
	long pop_directory(void);
	char *next_match_wildcard(long at_end);
	char *first(char *root_directory, char *local_directory);

public:
	ANT_directory_recursive_iterator();
	virtual ~ANT_directory_recursive_iterator();

	virtual char *first(char *wildcard);
	virtual char *next(void);

} ;


#endif __DIRECTORY_RECURSIVE_ITERATOR_H__
