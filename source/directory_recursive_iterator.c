/*
	DIRECTORY_RECURSIVE_ITERATOR.C
	------------------------------
*/
#ifdef _MSC_VER
	#include <shlwapi.h>
#else
	#include <unistd.h>
	#include <string.h>
	#include <fnmatch.h>
#endif
#include <stdio.h>
#include "disk.h"
#include "directory_iterator_internals.h"
#include "disk_directory.h"
#include "directory_recursive_iterator.h"

#define HANDLE_STACK_SIZE (PATH_MAX / 2)		/* because every second char must be a '\' */
#ifndef FALSE
	#define FALSE (0)
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

#ifndef _MSC_VER
	/*
		PATHMATCHSPEC()
		---------------
	*/
	static long PathMatchSpec(const char *str, const char *pattern)
	{
		char *fn = (char *)str;
		char *tmp = fn;

		while (*tmp != '\0') {
			if (*tmp == '/') fn = tmp + 1;
			tmp++;
		}

		return fnmatch(pattern, fn, FNM_PATHNAME) == 0; /* 0 = success */
	}
#endif


/*
	ANT_DIRECTORY_RECURSIVE_ITERATOR::ANT_DIRECTORY_RECURSIVE_ITERATOR()
	--------------------------------------------------------------------
*/
ANT_directory_recursive_iterator::ANT_directory_recursive_iterator()
{
handle_stack = new ANT_disk_directory [HANDLE_STACK_SIZE];
wildcard = new char [PATH_MAX];
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
	#ifdef _MSC_VER
		if (at_end == 0)
			{
			FindClose(file_list->handle);
			if (pop_directory())
				return next_match_wildcard(FindNextFile(file_list->handle, &internals->file_data));
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
	#else
		if (at_end == 0 || file_list->matching_files.gl_pathv[file_list->glob_index] == NULL)
			{
			globfree(&file_list->matching_files);
			if (pop_directory())
				{
				file_list->glob_index++;
				return next_match_wildcard(1);
				}
			else
				return NULL;
			}
		else if (file_list->matching_files.gl_pathv[file_list->glob_index][strlen(file_list->matching_files.gl_pathv[file_list->glob_index]) - 1] == '/')
			{
			if (true)
				{
				/* tmp is here as push_directory() will trash current file_list*/
				char *tmp=file_list->matching_files.gl_pathv[file_list->glob_index];
				dir = file_list->path;
				push_directory();
				if ((file = first(dir, tmp)) != NULL)
					return file;
				}
			}
		else
			if ((match = PathMatchSpec(file_list->matching_files.gl_pathv[file_list->glob_index], wildcard)) != 0)
				break;
		at_end = !(file_list->glob_index++ == file_list->matching_files.gl_pathc);
	#endif
	}

#ifdef _MSC_VER
	return internals->file_data.cFileName;
#else
	return file_list->matching_files.gl_pathv[file_list->glob_index++];
#endif
}

/*
	ANT_DIRECTORY_RECURSIVE_ITERATOR::FIRST()
	-----------------------------------------
*/
char *ANT_directory_recursive_iterator::first(char *root_directory, char *local_directory)
{
char path[PATH_MAX];

if (*local_directory != '\0' && *root_directory != '\0')
#ifdef _MSC_VER
	sprintf(file_list->path, "%s/%s", root_directory, local_directory);
#else
	sprintf(file_list->path, "%s", local_directory);
#endif
else if (*local_directory == '\0')
	strcpy(file_list->path, root_directory);
else if (*root_directory == '\0')
	strcpy(file_list->path, local_directory);
else
	strcpy(file_list->path, ".");

#ifdef _MSC_VER
	sprintf(path, "%s/*.*", file_list->path);
	file_list->handle = FindFirstFile(path, &internals->file_data);
	if (file_list->handle == INVALID_HANDLE_VALUE)
		return NULL;
#else
	sprintf(path, "%s*", file_list->path);
	glob(path, GLOB_MARK, NULL, &file_list->matching_files);
	file_list->glob_index = 0;

	if (file_list->matching_files.gl_pathc == 0) /* None found */
		return NULL;
#endif

return next_match_wildcard(1);
}

/*
	ANT_DIRECTORY_RECURSIVE_ITERATOR::FIRST()
	-----------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_recursive_iterator::first(ANT_directory_iterator_object *object, char *wildcard, long get_file)
{
char *got;

file_list = handle_stack;
strcpy(this->wildcard, wildcard);

#ifdef _MSC_VER
	GetCurrentDirectory(sizeof(path_buffer), path_buffer);
	if ((got = first(path_buffer, "")) == NULL)
		return NULL;
	sprintf(object->filename, "%s/%s", file_list->path, got);
#else
	/* the modification below would not affected the original way of reading files */
	long last_slash_idx = strlen(wildcard) - 1;
	char *last_char = wildcard + last_slash_idx; // searching backward
	char *slash = last_char;
	while (slash != wildcard)
		{
		if(*slash == '/')
			break;
		slash--;
		last_slash_idx--;
		}
	if (last_slash_idx <= 0) 		// the wildcard will be wildcard itself
		{
		getcwd(path_buffer, sizeof(path_buffer));
		sprintf(path_buffer, "%s/", path_buffer); /* As we will later use this to mark dirs */
		}
	else // the wildcard will be after slash
		{
		if (last_char == slash)
			strcpy(this->wildcard, "*");
		else
			{
			char *wildcard_start = slash;
			wildcard_start++;
			long wildcard_len = last_char - wildcard_start + 1;
			strncpy(this->wildcard, wildcard_start, wildcard_len);
			this->wildcard[wildcard_len] = '\0';
			}
		strncpy(path_buffer, wildcard, last_slash_idx + 1);
		}
	if ((got = first(path_buffer, "")) == NULL)
		return NULL;
	sprintf(object->filename, "%s", got);
#endif

if (get_file)
	object->file = ANT_disk::read_entire_file(object->filename, &object->length);
else
	{
	object->file = NULL;
	object->length = 0;
	}

return object;
}

/*
	ANT_DIRECTORY_RECURSIVE_ITERATOR::NEXT()
	----------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_recursive_iterator::next(ANT_directory_iterator_object *object, long get_file)
{
char *got;

#ifdef _MSC_VER
	if ((got = next_match_wildcard(FindNextFile(file_list->handle, &internals->file_data))) == NULL)
		return NULL;
	sprintf(object->filename, "%s/%s", file_list->path, got);
#else
	if ((got = next_match_wildcard(1)) == NULL)
		return NULL;
	sprintf(object->filename, "%s", got);
#endif

if (get_file)
	object->file = ANT_disk::read_entire_file(object->filename, &object->length);
else
	{
	object->file = NULL;
	object->length = 0;
	}

return object;
}

