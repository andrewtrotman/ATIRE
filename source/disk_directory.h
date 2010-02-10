/*
	DISK_DIRECTORY.H
	----------------
*/

#ifndef DISK_DIRECTORY_H_
#define DISK_DIRECTORY_H_

#include <stdlib.h>
#include <limits.h>

/*
	class ANT_DISK_DIRECTORY
	------------------------
*/
class ANT_disk_directory
{
public:
#ifdef _MSC_VER
	HANDLE handle;
	long first_time;
#else
	glob_t matching_files;
	unsigned int glob_index;
#endif
	char path[PATH_MAX];
} ;


#endif  /* DISK_DIRECTORY_H_ */
