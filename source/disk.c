/*
	DISK.C
	------
*/
#include <windows.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "disk.h"
#include "disk_internals.h"

#ifdef _MSC_DEV
	#define stat(a,b) _stat(a,b)
#endif

/*
	ANT_DISK::ANT_DISK()
	--------------------
*/
ANT_disk::ANT_disk()
{
internals = new ANT_disk_internals;
}

/*
	ANT_DISK::~ANT_DISK()
	---------------------
*/
ANT_disk::~ANT_disk()
{
delete internals;
}


/*
	ANT_DISK::READ_ENTIRE_FILE()
	----------------------------
*/
char *ANT_disk::read_entire_file(char *filename)
{
struct stat details;
char *block;
FILE *fp;

if (filename == NULL)
	return NULL;
if (stat(filename, &details) != 0)
	return NULL;
if (details.st_size == 0)
	return NULL;
if ((block = new char [details.st_size]) == NULL)
	return NULL;
if ((fp = fopen(filename, "rb")) == NULL)
	return NULL;
if (fread(block, details.st_size, 1, fp) != 1)
	{
	delete [] block;
	block = NULL;
	}
fclose(fp);
return block;
}

/*
	ANT_DISK::GET_FIRST_FILENAME()
	------------------------------
*/
char *ANT_disk::get_first_filename(char *wildcard)
{
if ((internals->file_list = FindFirstFile(wildcard, &internals->file_data)) == INVALID_HANDLE_VALUE)
	return NULL;
else
	return internals->file_data.cFileName;
}

/*
	ANT_DISK::GET_NEXT_FILENAME()
	-----------------------------
*/
char *ANT_disk::get_next_filename(void)
{
if (FindNextFile(internals->file_list, &internals->file_data) == 0)
	{
	FindClose(internals->file_list);
	return NULL;
	}
else
	return internals->file_data.cFileName;
}
