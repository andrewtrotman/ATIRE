/*
	DIRECTORY_RECURSIVE_ITERATOR.C
	------------------------------
*/
#include <shlwapi.h>
#include <stdio.h>
#include "disk_internals.h"
#include "disk_directory.h"
#include "directory_recursive_iterator.h"

#define HANDLE_STACK_SIZE (_MAX_PATH / 2)		/* because every second char must be a '\' */   

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
		FindClose(file_list->handle);
		if (pop_directory())
			return next();
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

	at_end = FindNextFile(file_list->handle, &internals->file_data);
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
	sprintf(file_list->path, "%s\\%s", root_directory, local_directory);
else if (*local_directory == '\0')
	strcpy(file_list->path, root_directory);
else if (*root_directory == '\0')
	strcpy(file_list->path, local_directory);
else
	strcpy(file_list->path, ".");

sprintf(path, "%s\\*.*", file_list->path);

file_list->handle = FindFirstFile(path, &internals->file_data);

if (file_list->handle == INVALID_HANDLE_VALUE)
	return NULL;
else
	return next_match_wildcard(1);
}

/*
	ANT_DIRECTORY_RECURSIVE_ITERATOR::FIRST()
	-----------------------------------------
*/
char *ANT_directory_recursive_iterator::first(char *wildcard)
{
char buffer[MAX_PATH];

file_list = handle_stack;
strcpy(this->wildcard, wildcard);

GetCurrentDirectory(sizeof(buffer), buffer);
return first(buffer, "");
}

/*
	ANT_DIRECTORY_RECURSIVE_ITERATOR::NEXT()
	----------------------------------------
*/
char *ANT_directory_recursive_iterator::next(void)
{
return next_match_wildcard(FindNextFile(file_list->handle, &internals->file_data));
}

