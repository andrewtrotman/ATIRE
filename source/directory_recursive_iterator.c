/*
	DIRECTORY_RECURSIVE_ITERATOR.C
	------------------------------
*/
#ifdef _MSC_VER
#include <shlwapi.h>
#else
#include <unistd.h>
#include <string.h>
#endif
#include <stdio.h>
#include "disk_internals.h"
#include "disk_directory.h"
#include "directory_recursive_iterator.h"

#define HANDLE_STACK_SIZE (MAX_PATH / 2)		/* because every second char must be a '\' */   
#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (! FALSE)
#endif


/*
	ANT_DIRECTORY_RECURSIVE_ITERATOR::ANT_DIRECTORY_RECURSIVE_ITERATOR()
	--------------------------------------------------------------------
*/
ANT_directory_recursive_iterator::ANT_directory_recursive_iterator()
{
handle_stack = new ANT_disk_directory [HANDLE_STACK_SIZE];
wildcard = new char [MAX_PATH];
file_list = handle_stack;
}

/*
	ANT_DIRECTORY_RECURSIVE_ITERATOR::~ANT_DIRECTORY_RECURSIVE_ITERATOR()
	---------------------------------------------------------------------
*/
ANT_directory_recursive_iterator::~ANT_directory_recursive_iterator()
{
delete [] handle_stack;
delete [] wildcard;
}

/*
	ANT_DIRECTORY_RECURSIVE_ITERATOR::PUSH_DIRECTORY()
	--------------------------------------------------
*/
long ANT_directory_recursive_iterator::push_directory(void)
{
if (file_list < handle_stack + HANDLE_STACK_SIZE)
	{
	file_list++;
	return TRUE;
	}
return FALSE;
}

/*
	ANT_DIRECTORY_RECURSIVE_ITERATOR::POP_DIRECTORY()
	-------------------------------------------------
*/
long ANT_directory_recursive_iterator::pop_directory(void)
{
if (file_list > handle_stack)
	{
	file_list--;
	return TRUE;
	}
return FALSE;
}

/*
	ANT_DIRECTORY_RECURSIVE_ITERATOR::NEXT_MATCH_WILDCARD()
	-------------------------------------------------------
*/
char *ANT_directory_recursive_iterator::next_match_wildcard(long at_end)
{
char *dir, *file;
long match = FALSE;

while (!match)
	{
	if (at_end == 0)
		{
#ifdef _MSC_VER
		FindClose(file_list->handle);
#else
		globfree(&internals->matching_files);
#endif
		if (pop_directory())
			{
#ifdef _MSC_VER
			return next_match_wildcard(FindNextFile(file_list->handle, &internals->file_data));
//			return next();
#else
			return next_match_wildcard(FindNextFile(file_list->fp, &internals->matching_files));
#endif
			}
		else
			return NULL;
		}
	else if (internals->file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
		if (!(strcmp(internals->file_data.cFileName, ".") == 0 || strcmp(internals->file_data.cFileName, "..") == 0))
			{
			dir = file_list->path;
			push_directory();
			if ((file = first(dir, internals->file_data.cFileName)) != NULL)
				return file;
			}
		}
	else 
		if ((match = PathMatchSpec(internals->file_data.cFileName, wildcard)) != 0)
			break;
#ifdef _MSC_VER
	at_end = FindNextFile(file_list->handle, &internals->file_data);
#else
    if (internals->glob_index == internals->matching_files.gl_pathc)
        at_end = NULL;
    else
        at_end = internals->matching_files.gl_pathv[internals->glob_index++];
#endif
	}

return internals->file_data.cFileName;
}

/*
	ANT_DIRECTORY_RECURSIVE_ITERATOR::FIRST()
	-----------------------------------------
*/
char *ANT_directory_recursive_iterator::first(char *root_directory, char *local_directory)
{
char path[MAX_PATH];

if (*local_directory != '\0' && *root_directory != '\0')
	sprintf(file_list->path, "%s/%s", root_directory, local_directory);
else if (*local_directory == '\0')
	strcpy(file_list->path, root_directory);
else if (*root_directory == '\0')
	strcpy(file_list->path, local_directory);
else
	strcpy(file_list->path, ".");

sprintf(path, "%s/*.*", file_list->path);

#ifdef _MSC_VER
file_list->handle = FindFirstFile(path, &internals->file_data);
if (file_list->handle == INVALID_HANDLE_VALUE)
	return NULL;
#else
glob(path, 0, NULL, &internals->matching_files);
internals->glob_index = 0;

if (internals->matching_files.gl_pathc == 0) /* None found */
	return NULL;
#endif
return next_match_wildcard(1);
}

/*
	ANT_DIRECTORY_RECURSIVE_ITERATOR::FIRST()
	-----------------------------------------
*/
char *ANT_directory_recursive_iterator::first(char *wildcard)
{
char *got;

file_list = handle_stack;
strcpy(this->wildcard, wildcard);

#ifdef _MSC_VER
GetCurrentDirectory(sizeof(path_buffer), path_buffer);
#else
getcwd(path_buffer, sizeof(path_buffer));
#endif
if ((got = first(path_buffer, "")) == NULL)
	return NULL;

sprintf(path_buffer, "%s/%s", file_list->path, got);
return path_buffer;
}

/*
	ANT_DIRECTORY_RECURSIVE_ITERATOR::NEXT()
	----------------------------------------
*/
char *ANT_directory_recursive_iterator::next(void)
{
char *got;

#ifdef _MSC_VER
if ((got = next_match_wildcard(FindNextFile(file_list->handle, &internals->file_data))) == NULL)
	return NULL;
#else
/* Not sure about the next_match_wildcard bit */
/* 
    FindNextFile ->
    internals->matching_files.gl_pathv[internals->glob_index++]
*/

if (internals->glob_index == internals->matching_files.gl_pathc)
	return NULL;
else
    got = next_match_wildcard(internals->matching_files.gl_pathv[internals->glob_index++]);
#endif

sprintf(path_buffer, "%s/%s", file_list->path, got);
return path_buffer;
}

