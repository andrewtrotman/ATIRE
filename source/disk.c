/*
	DISK.C
	------
*/
#include <windows.h>
#include <new.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "disk.h"
#include "disk_internals.h"

#ifdef _MSC_VER
	#define stat _stat64
	#define fstat _fstat64
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

if ((fp = fopen(filename, "rb")) == NULL)
	return NULL;

if (fstat(fileno(fp), &details) != 0)
	return NULL;

if (details.st_size == 0)
	return NULL;

if ((block = new (std::nothrow) char [(long)(details.st_size + 1)]) == NULL)
	return NULL;

if (fread(block, (long)details.st_size, 1, fp) != 1)
	{
	delete [] block;
	block = NULL;
	}
else
	block[details.st_size] = '\0';

fclose(fp);
return block;
}

/*
	ANT_DISK::CONSTRUCT_FULL_PATH()
	-------------------------------
*/
inline char *ANT_disk::construct_full_path(char *filename)
{
strcpy(internals->fully_qualified_filename, internals->pathname);
strcat(internals->fully_qualified_filename, filename);

return internals->fully_qualified_filename;
}

/*
	ANT_DISK::GET_FIRST_FILENAME()
	------------------------------
*/
char *ANT_disk::get_first_filename(char *wildcard)
{
char *slash, *colon, *backslash, *max;

if ((internals->file_list = FindFirstFile(wildcard, &internals->file_data)) == INVALID_HANDLE_VALUE)
	return NULL;

strcpy(internals->pathname, wildcard);

slash = strrchr(internals->pathname, '/');
backslash = strrchr(internals->pathname, '\\');
colon = strrchr(internals->pathname, ':');

max = internals->pathname - 1;
if (slash > max)
	max = slash;
if (backslash > max)
	max = backslash;
if (colon > max)
	max = colon;

*(max + 1) = '\0';

return construct_full_path(internals->file_data.cFileName);
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

return construct_full_path(internals->file_data.cFileName);
}


