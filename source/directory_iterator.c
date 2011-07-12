/*
	DIRECTORY_ITERATOR.C
	--------------------
*/
#include "disk.h"
#include "directory_iterator.h"
#include "directory_iterator_internals.h"
#include <string.h>
#include "str.h"

/*
	ANT_DIRECTORY_ITERATOR::ANT_DIRECTORY_ITERATOR()
	------------------------------------------------
*/
ANT_directory_iterator::ANT_directory_iterator(char *wildcard, long get_file)
{
strncpy(this->wildcard, wildcard, PATH_MAX);
this->wildcard[PATH_MAX - 1] = '\0';

this->get_file = get_file;

internals = new ANT_directory_iterator_internals;
}

/*
	ANT_DIRECTORY_ITERATOR::~ANT_DIRECTORY_ITERATOR()
	-------------------------------------------------
*/
ANT_directory_iterator::~ANT_directory_iterator()
{
delete internals;
}

/*
	ANT_DIRECTORY_ITERATOR::CONSTRUCT_FULL_PATH()
	---------------------------------------------
*/
inline char *ANT_directory_iterator::construct_full_path(char *filename)
{
char * destination = new char[strlen(internals->pathname) + strlen(filename) + 1];
strcpy(destination, internals->pathname);
strcat(destination, filename);

return destination;
}

/*
	ANT_DIRECTORY_ITERATOR::FIRST()
	-------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator::first(ANT_directory_iterator_object *object)
{
char *slash, *colon, *backslash, *max;

#ifdef _MSC_VER
	if ((internals->file_list = FindFirstFile(wildcard, &internals->file_data)) == INVALID_HANDLE_VALUE)
		return NULL;

	while (internals->file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		if (FindNextFile(internals->file_list, &internals->file_data) == 0)
			{
			FindClose(internals->file_list);
			return NULL;
			}
#else
	glob(wildcard, 0, NULL, &internals->matching_files);
	internals->glob_index = 0;

	if (internals->matching_files.gl_pathc == 0) /* None found */
		return NULL;
#endif

strcpy(internals->pathname, wildcard);

slash = strrchr(internals->pathname, '/');
backslash = strrchr(internals->pathname, '\\');
colon = strrchr(internals->pathname, ':');

max = internals->pathname;
if (slash > max)
	max = slash + 1;
if (backslash > max)
	max = backslash + 1;
if (colon > max)
	max = colon + 1;

*max = '\0';

#ifdef _MSC_VER
	object->filename = construct_full_path(internals->file_data.cFileName);
#else
	object->filename = strnew(internals->matching_files.gl_pathv[internals->glob_index++]);
#endif
if (get_file)
	object->file = ANT_disk::read_entire_file(object->filename, &object->length);
else
	{
	object->length = 0;
	object->file = NULL;
	}

return object;
}

/*
	ANT_DIRECTORY_ITERATOR::NEXT()
	------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator::next(ANT_directory_iterator_object *object)
{
#ifdef _MSC_VER
	do
		if (FindNextFile(internals->file_list, &internals->file_data) == 0)
			{
			FindClose(internals->file_list);
			return NULL;
			}
	while (internals->file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

	object->filename = construct_full_path(internals->file_data.cFileName);
#else
	if (internals->glob_index == internals->matching_files.gl_pathc)
		{
		globfree(&internals->matching_files);
		return NULL;
		}

	object->filename = strnew(internals->matching_files.gl_pathv[internals->glob_index++]);
#endif

if (get_file)
	object->file = ANT_disk::read_entire_file(object->filename, &object->length);
else
	{
	object->length = 0;
	object->file = NULL;
	}

return object;
}


