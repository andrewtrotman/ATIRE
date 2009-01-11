/*
	DISK.C
	------
*/
/* #include <windows.h> */
#include <new>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <glob.h>
#include "disk.h"
#include "disk_internals.h"

#ifdef _MSC_VER
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
 try {
     block = new char [details.st_size + 1];
 } catch (...) {
     return NULL;
 }
if ((fp = fopen(filename, "rb")) == NULL)
	return NULL;
if (fread(block, details.st_size, 1, fp) != 1)
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

glob(wildcard, 0, NULL, &internals->matching_files);
internals->glob_index = 0;

if (internals->matching_files.gl_pathc == 0) /* None found */
    return NULL;

/* Cut string to last of '/' '\\' ':' */
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



return internals->matching_files.gl_pathv[internals->glob_index++];
}

/*
	ANT_DISK::GET_NEXT_FILENAME()
	-----------------------------
*/
char *ANT_disk::get_next_filename(void)
{
if (internals->glob_index == internals->matching_files.gl_pathc)
    {
    globfree(&internals->matching_files);
    return NULL;
    }

return internals->matching_files.gl_pathv[internals->glob_index++];
}
