/*
	DIRECTORY_ITERATOR_RECURSIVE.H
	------------------------------
*/
#ifndef DIRECTORY_ITERATOR_RECURSIVE_H_
#define DIRECTORY_ITERATOR_RECURSIVE_H_


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
	class ANT_DIRECTORY_ITERATOR_RECURSIVE
	--------------------------------------
*/
class ANT_directory_iterator_recursive : public ANT_directory_iterator
{
private:
	ANT_disk_directory *handle_stack;
	ANT_disk_directory *file_list;
	char path_buffer[PATH_MAX];

private:
	long push_directory(void);
	long pop_directory(void);
	char *next_match_wildcard(void);
#ifdef _MSC_VER
	long get_next_candidate(void);
#else
	char *first_match_wildcard(char *root_directory);
	static long PathMatchSpec(const char *str, const char *pattern);
#endif

public:
	ANT_directory_iterator_recursive(const char *the_wildcard, long get_file = 0);
	virtual ~ANT_directory_iterator_recursive();

	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object);
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);
} ;



#endif /* DIRECTORY_ITERATOR_RECURSIVE_H_ */
