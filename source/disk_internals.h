/*
	DISK_INTERNALS.H
	----------------
*/
#ifndef DISK_INTERNALS_H_
#define DISK_INTERNALS_H_

#ifdef _MSC_VER
	#include <windows.h>
	#define PATH_MAX MAX_PATH
#else
	#include <limits.h>
	#include <glob.h>
	#include <stdio.h>
#endif

/*
	class ANT_DISK_INTERNALS
	------------------------
*/
class ANT_disk_internals
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
	ANT_disk_internals();
	virtual ~ANT_disk_internals() {}
} ;

#endif  /* DISK_INTERNALS_H_ */
