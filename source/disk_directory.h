/*
	DISK_DIRECTORY.H
	----------------
*/

#ifndef __DISK_DIRECTORY_H__
#define __DISK_DIRECTORY_H__

#include <stdlib.h>

#define MAX_PATH_LENGTH _MAX_PATH

/*
	class ANT_DISK_DIRECTORY
	------------------------
*/
class ANT_disk_directory
{
public:
	HANDLE handle;
	char path[MAX_PATH_LENGTH];
} ;


#endif __DISK_DIRECTORY_H__
