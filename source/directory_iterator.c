/*
	DIRECTORY_ITERATOR.C
	--------------------
*/
#include "disk.h"
#include "directory_iterator.h"
#include "directory_iterator_internals.h"

/*
	ANT_DIRECTORY_ITERATOR::ANT_DIRECTORY_ITERATOR()
	------------------------------------------------
*/
ANT_directory_iterator::ANT_directory_iterator()
{
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
strcpy(internals->fully_qualified_filename, internals->pathname);
strcat(internals->fully_qualified_filename, filename);

return internals->fully_qualified_filename;
}

/*
	ANT_DIRECTORY_ITERATOR::FIRST()
	-------------------------------
*/
char *ANT_directory_iterator::first(char *wildcard)
{
char *slash, *colon, *backslash, *max;

#ifdef _MSC_VER
	if ((internals->file_list = FindFirstFile(wildcard, &internals->file_data)) == INVALID_HANDLE_VALUE)
		return NULL;
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

max = internals->pathname - 1;
if (slash > max)
	max = slash;
if (backslash > max)
	max = backslash;
if (colon > max)
	max = colon;

*(max + 1) = '\0';

#ifdef _MSC_VER
	return construct_full_path(internals->file_data.cFileName);
#else
	return internals->matching_files.gl_pathv[internals->glob_index++];
#endif
}

/*
	ANT_DIRECTORY_ITERATOR::NEXT()
	------------------------------
*/
char *ANT_directory_iterator::next(void)
{
#ifdef _MSC_VER
	if (FindNextFile(internals->file_list, &internals->file_data) == 0)
		{
		FindClose(internals->file_list);
		return NULL;
		}

	return construct_full_path(internals->file_data.cFileName);
#else
	if (internals->glob_index == internals->matching_files.gl_pathc)
		{
		globfree(&internals->matching_files);
		return NULL;
		}

	return internals->matching_files.gl_pathv[internals->glob_index++];
#endif
}

/*
	ANT_DIRECTORY_ITERATOR::READ_ENTIRE_FILE()
	------------------------------------------
*/
char *ANT_directory_iterator::read_entire_file(long long *len)
{
return ANT_disk::read_entire_file(internals->fully_qualified_filename, len);
}
