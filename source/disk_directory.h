/*
	DISK_DIRECTORY.H
	----------------
*/

#ifndef __DISK_DIRECTORY_H__
#define __DISK_DIRECTORY_H__

#include <stdlib.h>

#ifdef _MSC_VER
#define MAX_PATH_LENGTH _MAX_PATH
#else
#define MAX_PATH_LENGTH MAX_PATH
#endif
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
	char path[MAX_PATH_LENGTH];
} ;


#endif __DISK_DIRECTORY_H__
