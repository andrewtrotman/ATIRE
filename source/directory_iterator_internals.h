/*
	DIRECTORY_ITERATOR_INTERNALS.H
	------------------------------
*/
#ifndef DIRECTORY_ITERATOR_INTERNALS_H_
#define DIRECTORY_ITERATOR_INTERNALS_H_

#ifdef _MSC_VER
	#include <windows.h>
	#define PATH_MAX MAX_PATH
#else
	#include <limits.h>
	#include <glob.h>
	#include <stdio.h>
#endif

/*
	class ANT_DIRECTORY_ITERATOR_INTERNALS
	--------------------------------------
*/
class ANT_directory_iterator_internals
{
public:
#ifdef _MSC_VER
	HANDLE file_list;
	WIN32_FIND_DATA file_data;
#else
	FILE *file_list;
	glob_t matching_files;
	unsigned int glob_index;
#endif
	char pathname[PATH_MAX];
	char fully_qualified_filename[PATH_MAX];

public:
	ANT_directory_iterator_internals();
	virtual ~ANT_directory_iterator_internals() {}
} ;


#endif /* DIRECTORY_ITERATOR_INTERNALS_H_ */
