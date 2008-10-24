/*
	DISK_INTERNALS.H
	----------------
*/
#ifndef __DISK_INTERNALS_H__
#define __DISK_INTERNALS_H__

#include <windows.h>

class ANT_disk_internals
{
public:
	HANDLE file_list;
	WIN32_FIND_DATA file_data;
	char pathname[MAX_PATH];
	char fully_qualified_filename[MAX_PATH];

public:
	ANT_disk_internals();
	~ANT_disk_internals();
} ;

#endif __DISK_INTERNALS_H__
