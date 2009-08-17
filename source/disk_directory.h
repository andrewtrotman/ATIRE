/*
	DISK_DIRECTORY.H
	----------------
*/

#ifndef __DISK_DIRECTORY_H__
#define __DISK_DIRECTORY_H__

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
#else
	glob_t matching_files;
	unsigned int glob_index;
#endif
	char path[PATH_MAX];
} ;


#endif __DISK_DIRECTORY_H__
