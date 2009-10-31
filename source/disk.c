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
#include "file_internals.h"

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
			if ((block = new (std::nothrow) char [(size_t)(details.st_size + 1)]) != NULL)		// +1 for the '\0' on the end
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

