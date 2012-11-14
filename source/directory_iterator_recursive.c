/*
	DIRECTORY_ITERATOR_RECURSIVE.C
	------------------------------
*/
#ifdef _MSC_VER
	#include <shlwapi.h>
#else
	#include <unistd.h>
	#include <string.h>
	#include <fnmatch.h>
	#include <glob.h>
#endif
#include <stdio.h>
#include "str.h"
#include "disk.h"
#include "directory_iterator_internals.h"
#include "disk_directory.h"
#include "directory_iterator_recursive.h"

#define HANDLE_STACK_SIZE (PATH_MAX / 2)		/* because every second char must be a '\' */

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_DIRECTORY_ITERATOR_RECURSIVE::ANT_DIRECTORY_ITERATOR_RECURSIVE()
	--------------------------------------------------------------------
*/
ANT_directory_iterator_recursive::ANT_directory_iterator_recursive(const char *the_wildcard, long get_file) : ANT_directory_iterator((char *)the_wildcard, get_file)
{
handle_stack = new ANT_disk_directory [HANDLE_STACK_SIZE];
strncpy(wildcard, the_wildcard, PATH_MAX);
wildcard[PATH_MAX - 1] = '\0';
file_list = handle_stack;
#ifdef _MSC_VER
file_list->first_time = TRUE;
#endif
}

/*
	ANT_DIRECTORY_ITERATOR_RECURSIVE::~ANT_DIRECTORY_ITERATOR_RECURSIVE()
	---------------------------------------------------------------------
*/
ANT_directory_iterator_recursive::~ANT_directory_iterator_recursive()
{
delete [] handle_stack;
}

/*
	ANT_DIRECTORY_ITERATOR_RECURSIVE::PUSH_DIRECTORY()
	--------------------------------------------------
*/
long ANT_directory_iterator_recursive::push_directory(void)
{
if (file_list < handle_stack + HANDLE_STACK_SIZE)
	{
	file_list++;
	#ifdef _MSC_VER
	file_list->first_time = TRUE;
	#endif
	return TRUE;
	}
return FALSE;
}

/*
	ANT_DIRECTORY_ITERATOR_RECURSIVE::POP_DIRECTORY()
	-------------------------------------------------
*/
long ANT_directory_iterator_recursive::pop_directory(void)
{
if (file_list > handle_stack)
	{
	file_list--;
	return TRUE;
	}
return FALSE;
}


#ifdef _MSC_VER
	/*
		ANT_DIRECTORY_ITERATOR_RECURSIVE::GET_NEXT_CANDIDATE()
		------------------------------------------------------
	*/
	long ANT_directory_iterator_recursive::get_next_candidate(void)
	{
	long success;

	do
		{
		success = TRUE;
		if (file_list->first_time)
			{
			file_list->first_time = FALSE;
			if ((file_list->handle = FindFirstFile(file_list->path, &internals->file_data)) == INVALID_HANDLE_VALUE)
				success = FALSE;
			}
		else
			if (FindNextFile(file_list->handle, &internals->file_data) == 0)
				success = FALSE;

		if (!success)
			{
			FindClose(file_list->handle);
			if (pop_directory() == FALSE)
				return FALSE;
			}
		}
	while (!success);

	return TRUE;
	}
#else
	/*
		ANT_DIRECTORY_ITERATOR_RECURSIVE::PATHMATCHSPEC()
		-------------------------------------------------
	*/
	long ANT_directory_iterator_recursive::PathMatchSpec(const char *str, const char *pattern)
	{
	char *fn = (char *)str;
	char *tmp = fn;

	while (*tmp != '\0')
		{
		if (*tmp == '/')
			fn = tmp + 1;
		tmp++;
		}

	return fnmatch(pattern, fn, FNM_PATHNAME) == 0; /* 0 = success */
	}

	/*
		ANT_DIRECTORY_ITERATOR_RECURSIVE::FIRST()
		-----------------------------------------
	*/
	char *ANT_directory_iterator_recursive::first_match_wildcard(char *root_directory)
	{
	char path[PATH_MAX];

	if (*root_directory != '\0')
		strcpy(file_list->path, root_directory);
	else
		strcpy(file_list->path, ".");

	sprintf(path, "%s*", file_list->path); // that is the wildcard used for
	switch(glob(path, GLOB_MARK, NULL, &file_list->matching_files))
	{
		case 0:
			break;
		case GLOB_NOSPACE:
			fprintf(stderr, "ERROR: glob, Out of memory\n");
			return NULL;
		case GLOB_ABORTED:
			fprintf(stderr, "ERROR: glob, Reading error\n");
			return NULL;
		case GLOB_NOMATCH:
			fprintf(stderr, "WARNING: glob, No files found under path \"%s\"\n", file_list->path);
			return NULL;
		default:
			break;
	}

	file_list->glob_index = 0;

	return next_match_wildcard();
	}
#endif

/*
	ANT_DIRECTORY_ITERATOR_RECURSIVE::NEXT_MATCH_WILDCARD()
	-------------------------------------------------------
*/
char *ANT_directory_iterator_recursive::next_match_wildcard(void)
{
#ifdef _MSC_VER
	ANT_disk_directory *current_file_list;
	size_t path_length;

	while (get_next_candidate())
		{
		if (internals->file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
			if (!(strcmp(internals->file_data.cFileName, ".") == 0 || strcmp(internals->file_data.cFileName, "..") == 0))
				{
				current_file_list = file_list;
				push_directory();
				path_length = strlen(current_file_list->path) - 4;
				sprintf(file_list->path, "%*.*s/%s/*.*", (int)path_length, (int)path_length, current_file_list->path, internals->file_data.cFileName);
				}
			}
		else
			if (PathMatchSpec(internals->file_data.cFileName, wildcard))
				return internals->file_data.cFileName;
		}
	return NULL;
#else
	char *file;
	long match = FALSE, at_end = 1;

	while (!match)
		{
		if (at_end == 0 || file_list->glob_index >= file_list->matching_files.gl_pathc)
			{
			globfree(&file_list->matching_files);
			if (pop_directory())
				{
				file_list->glob_index++;
				return next_match_wildcard();
				}
			else
				return NULL;
			}
		// check if the matching path is a directory
		else if (file_list->matching_files.gl_pathv[file_list->glob_index][strlen(file_list->matching_files.gl_pathv[file_list->glob_index]) - 1] == '/')
			{
			if (true)
				{
				/* tmp is here as push_directory() will trash current file_list*/
				char *tmp=file_list->matching_files.gl_pathv[file_list->glob_index];
				push_directory();
				if ((file = first_match_wildcard(tmp)) != NULL)
					return file;
				}
			}
		else
			if ((match = PathMatchSpec(file_list->matching_files.gl_pathv[file_list->glob_index], wildcard)) != 0)
				break;
		at_end = !(file_list->glob_index++ == file_list->matching_files.gl_pathc);
		}
	return file_list->matching_files.gl_pathv[file_list->glob_index++];
#endif
}


/*
	ANT_DIRECTORY_ITERATOR_RECURSIVE::FIRST()
	-----------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_recursive::first(ANT_directory_iterator_object *object)
{
file_list = handle_stack;

#ifdef _MSC_VER
	file_list->first_time = TRUE;
	GetCurrentDirectory(sizeof(file_list->path), file_list->path);
	strcat(file_list->path, "/*.*");

	return next(object);
#else
	/* the modification below would not affected the original way of reading files */
	long last_slash_idx = strlen(wildcard) - 1;
	char *last_char = wildcard + last_slash_idx; // searching backward
	char *slash = last_char;
	char *got;

	if (!ANT_disk::is_directory(wildcard))
		{
		while (slash != wildcard)
			{
			if(*slash == '/')
				break;
			slash--;
			last_slash_idx--;
			}

		if (last_slash_idx <= 0) 		// the wildcard will be wildcard itself
			{
			if (getcwd(path_buffer, sizeof(path_buffer)) == NULL)
				return NULL;

			sprintf(path_buffer, "%s/", path_buffer); /* As we will later use this to mark dirs */
			}
		else // the wildcard will be after slash
			{
			strncpy(path_buffer, wildcard, last_slash_idx + 1);
			path_buffer[last_slash_idx+1] = '\0';
			if (last_char == slash)
				strcpy(this->wildcard, "*");
			else
				{
				char *wildcard_start = slash;
				wildcard_start++;
				long wildcard_len = last_char - wildcard_start + 1;
				// trim the wildcard path string to contain only the wildcard characters
				strncpy(this->wildcard, wildcard_start, wildcard_len);
				this->wildcard[wildcard_len] = '\0';
				}
			}
		}
	else
		{
		strcpy(path_buffer, wildcard);
		strcpy(wildcard, "*");
		}

	if ((got = first_match_wildcard(path_buffer)) == NULL)
		return NULL;

	object->filename = strnew(got);

	if (get_file)
		object->file = ANT_disk::read_entire_file(object->filename, &object->length);
	else
		{
		object->file = NULL;
		object->length = 0;
		}

	return object;
#endif
}

/*
	ANT_DIRECTORY_ITERATOR_RECURSIVE::NEXT()
	----------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_recursive::next(ANT_directory_iterator_object *object)
{
char *got;

#ifdef _MSC_VER
	size_t path_length;

	if ((got = next_match_wildcard()) == NULL)
		return NULL;
	path_length = strlen(file_list->path) - 4;
	sprintf(object->filename, "%*.*s/%s", (int)path_length, (int)path_length, file_list->path, got);
#else
	if ((got = next_match_wildcard()) == NULL)
		return NULL;
	object->filename = strnew(got);
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

