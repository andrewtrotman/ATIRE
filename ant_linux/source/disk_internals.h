/*
	DISK_INTERNALS.H
	----------------
*/
#ifndef __DISK_INTERNALS_H__
#define __DISK_INTERNALS_H__

/* #include <windows.h> */
#include <stdio.h>
#include <glob.h>

class ANT_disk_internals
{
public:
	FILE *file_list;
    glob_t matching_files;
    unsigned int glob_index;
	char pathname[FILENAME_MAX];
	char fully_qualified_filename[FILENAME_MAX];

public:
	ANT_disk_internals();
	~ANT_disk_internals();
} ;

#endif /* __DISK_INTERNALS_H__ */
