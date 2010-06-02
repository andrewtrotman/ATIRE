/*
 * sys_file.cpp
 *
 *  Created on: Aug 11, 2009
 *      Author: monfee
 */

#include "sys_file.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <stdexcept>
#include <string>

#if defined(WINDOWS) || defined(_WIN32) || defined(_WIN64)
	const char *sys_file::SEPARATOR = "\\";
	const char *sys_file::DEFAULT_PATTERN = "*.*";
#else
	const char *sys_file::SEPARATOR = "/";
	const char *sys_file::DEFAULT_PATTERN = "*";
#endif

sys_file::sys_file(const char *pattern) : pattern_(pattern)
{
	init();
}

sys_file::sys_file()
{
#if defined(WINDOWS) || defined(_WIN32) || defined(_WIN64)
	pattern_ = "*.*";
#else
	pattern_ = "*";
#endif
	init();
}

sys_file::~sys_file()
{
	free();
}

void sys_file::init()
{
	data_ = NULL;
	list_count_ = 0;
}

void sys_file::free()
{
	if (data_) {
		globfree( data_ );
		delete data_;
		data_ = NULL;
		list_count_ = 0;
	}
}
/*
	ANT_DISK::READ_ENTIRE_FILE()
	----------------------------
*/
char *sys_file::read_entire_file(const char *filename, long long *file_length)
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
char **sys_file::buffer_to_list(char *buffer, long long *lines)
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

bool sys_file::exist(const char *name) {
	struct stat st;
	return stat(&st, name) == 0;
}

size_t sys_file::size(const char *name)
{
	struct stat st;
	stat(&st, name);
	return st.st_size;
}

bool sys_file::isdir(const char *name) {
	struct stat st;
	if( stat(&st, name) == 0)
		return S_ISDIR(st.st_mode);
	return false;
}

char **sys_file::list(char *dir, char **sys_files_list, bool recursive)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir)) == NULL) {
    	// TODO throw a error exception
       // cout << "Error(" << errno << ") opening " << dir << endl;
    	return NULL;
    }

    while ((dirp = readdir(dp)) != NULL) {
    	std::string temp_str(dirp->d_name);
    	if (temp_str == "." || temp_str == "..")
    		continue;

    	// TODO
    	char *in_dir = dir;
    	//in_dir.push_back(SEPARATOR);
    	//in_dir.append(dirp->d_name);
    	if( dirp->d_type == DT_DIR && recursive) {
    		chdir(in_dir);
    	    list(in_dir, sys_files_list, recursive);
    	    chdir( ".." );
    	} //else
    		//sys_files.push_back(in_dir);
    }
    closedir(dp);
    return sys_files_list;
}

void sys_file::list(const char *name)
{
	char pathfile[PATH_MAX];
	char path[PATH_MAX];
	strcpy(pathfile, name);
	strcpy(path, name);
	stat(&stat_, pathfile);

	bool isdir = false;
	bool has_separator = strlen(name) > strlen(SEPARATOR) && strncmp(name + strlen(name) - strlen(SEPARATOR), SEPARATOR, strlen(SEPARATOR)) == 0;

	if (isdir = S_ISDIR(stat_.st_mode)) {
		if (!has_separator)
			strcat(pathfile, SEPARATOR);
		strcat(pathfile, pattern_);
	}

	free();
	data_ = new glob_t;

	int ret = 0;

	ret = glob(pathfile, 0);

	// append the directory
	if (/*ret == GLOB_NOMATCH && */isdir) {
		strcat(path, SEPARATOR);
		strcat(path, DEFAULT_PATTERN);
		ret = glob(path, GLOB_ONLYDIR | GLOB_APPEND);
	}

	if (data_->gl_pathc == 0)
		fprintf( stderr, "No %s found\n", pathfile );
}

int sys_file::glob(const char *name, int flag)
{
	int ret = 0;
	switch( ret = ::glob(name, flag, NULL, data_ ) )
	{
		case 0:
			break;
		case GLOB_NOSPACE:
			fprintf( stderr, "Out of memory\n" );
			break;
		case GLOB_ABORTED:
			fprintf( stderr, "Reading error\n" );
			break;
		case GLOB_NOMATCH:
//			fprintf( stderr, "No %s found\n", pathfile );
//			break;
		default:
			break;
	}
}

const char *sys_file::next()
{
	if (list_count_ < data_->gl_pathc)
		return data_->gl_pathv[list_count_++];
	return NULL;
}

int sys_file::stat(struct stat *st, const char *name)
{
	int ret = 0;
	ret = ::stat( name, st );
//	if( () == -1 )
//		throw std::runtime_error(std::string(name) + ": not exists!");
	return ret;
}

int sys_file::create_directory(const char *name)
{
	return mkdir(name, 0777);
}
