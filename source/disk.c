/*
	DISK.C
	------
*/
#ifdef _MSC_VER
	#include <windows.h>
    #include <direct.h>
#else
	#include <string.h>
#endif
#include <new>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "disk.h"
#include "disk_internals.h"
#include "disk_directory.h"
#include "file_internals.h"

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
char *ANT_disk::read_entire_file(char *filename, long long *file_length)
{
long long unused;
char *block = NULL;
#ifdef _MSC_VER
	HANDLE fp;
	LARGE_INTEGER details;
#else
	FILE *fp;
	struct stat details;
#endif

if (filename == NULL)
	return NULL;

if (file_length == NULL)
	file_length = &unused;

#ifdef _MSC_VER
	fp = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fp == INVALID_HANDLE_VALUE)
		return NULL;

	if (GetFileSizeEx(fp, &details) != 0)
		if ((*file_length = details.QuadPart) != 0)
			if ((block = new (std::nothrow) char [(size_t)(details.QuadPart + 1)]) != NULL)		// +1 for the '\0' on the end
				{
				if (ANT_file_internals::read_file_64(fp, block, details.QuadPart) != 0)
					block[details.QuadPart] = '\0';
				else
					{
					delete [] block;
					block = NULL;
					}
				}

	CloseHandle(fp);
#else
	if ((fp = fopen(filename, "rb")) == NULL)
		return NULL;

	if (fstat(fileno(fp), &details) == 0)
		if ((*file_length = details.st_size) != 0)
			if ((block = new (std::nothrow) char [(long)(details.st_size + 1)]) != NULL)		// +1 for the '\0' on the end
				if (fread(block, (long)details.st_size, 1, fp) == 1)
					block[details.st_size] = '\0';
				else
					{
					delete [] block;
					block = NULL;
					}
	fclose(fp);
#endif

return block;
}

/*
	ANT_DISK::BUFFER_TO_LIST()
	--------------------------
*/
char **ANT_disk::buffer_to_list(char *buffer, long long *lines)
{
char *pos, **line_list, **current_line;
long n_frequency, r_frequency;

n_frequency = r_frequency = 0;
for (pos = buffer; *pos != '\0'; pos++)
	if (*pos == '\n')
		n_frequency++;
	else if (*pos == '\r')
		r_frequency++;

*lines = r_frequency > n_frequency ? r_frequency : n_frequency;
current_line = line_list = new (std::nothrow) char * [(size_t)(*lines + 2)]; 		// +1 in case the last line has no \n; +1 for a NULL at the end of the list

if (line_list == NULL)		// out of memory!
	return NULL;

*current_line++ = pos = buffer;
while (*pos != '\0')
	{
	if (*pos == '\n' || *pos == '\r')
		{
		*pos++ = '\0';
		while (*pos == '\n' || *pos == '\r')
			pos++;
		*current_line++ = pos;
		}
	else
		pos++;
	}
*current_line = NULL;
*lines = current_line - line_list - 1;		// the true number of lines

return line_list;
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
	ANT_DISK::GET_NEXT_FILENAME()
	-----------------------------
*/
char *ANT_disk::get_next_filename(void)
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
	ANT_DISK::READ_ENTIRE_FILE()
	----------------------------
*/
char *ANT_disk::read_entire_file(long long *len)
{
return read_entire_file(internals->fully_qualified_filename, len);
}
