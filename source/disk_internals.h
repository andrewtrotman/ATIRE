/*
	DISK_INTERNALS.H
	----------------
*/
#ifndef __DISK_INTERNALS_H__
#define __DISK_INTERNALS_H__

#ifdef _MSC_VER
	#include <windows.h>
#elif defined(__APPLE__)
	#include <sys/syslimits.h>
#elif defined(__linux__)
	#include <linux/limits.h>
#endif
#ifndef _MSC_VER
	#include <glob.h>
        #include <stdio.h>
	#define MAX_PATH PATH_MAX
#endif


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
	char pathname[MAX_PATH];
	char fully_qualified_filename[MAX_PATH];

public:
	ANT_disk_internals();
	~ANT_disk_internals();
} ;

#endif __DISK_INTERNALS_H__
