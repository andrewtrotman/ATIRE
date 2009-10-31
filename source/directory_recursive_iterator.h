/*
	DIRECTORY_RECURSIVE_ITERATOR.H
	------------------------------
*/
#ifndef DIRECTORY_RECURSIVE_ITERATOR_H_
#define DIRECTORY_RECURSIVE_ITERATOR_H_

#include <stdlib.h>
#include "disk.h"
#include "directory_iterator.h"

#ifdef _MSC_VER
	#include <windows.h>
	#define PATH_MAX MAX_PATH
#else
	#include <limits.h>
#endif

class ANT_disk_directory;

/*
	class ANT_DIRECTORY_RECURSIVE_ITERATOR
	--------------------------------------
*/
class ANT_directory_recursive_iterator : public ANT_directory_iterator
{
private:
	ANT_disk_directory *handle_stack;
	ANT_disk_directory *file_list;
	char *wildcard;
	char path_buffer[PATH_MAX];

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
	virtual char *read_entire_file(long long *len = 0) { return ANT_disk::read_entire_file(path_buffer, len); }
} ;


#endif  /* DIRECTORY_RECURSIVE_ITERATOR_H_ */
