/*
 * sys_file.h
 *
 *  Created on: Aug 11, 2009
 *      Author: monfee
 */

#ifndef SYS_FILE_H_
#define SYS_FILE_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glob.h>

class sys_file
{
private:
	long			list_count_;

protected:
	const char 		*pattern_;
	struct stat 	stat_;
	bool			is_diretory_;
	glob_t 			*data_;

public:
	static const char*	SEPARATOR;
	static const char*	DEFAULT_PATTERN;

private:
	void free();
	void init();
	int glob(const char *name, int flag);

public:
	sys_file(const char *name);
	sys_file();
	virtual ~sys_file();

	//virtual char *first();
	virtual const char *next() ;

	static char *read_entire_file(const char *filename, long long *len = 0);
	static char **buffer_to_list(char *buffer, long long *lines);

	static bool exist(const char *name);
	static size_t size(const char *name);
	static bool isdir(const char *name);
	static int stat(struct stat *st, const char *name);
	static int create_directory(const char *name);

	char **list(char *dir, char **files_list, bool recursive = false);
	virtual void list(const char *name);

	void pattern(const char *wildcard) { pattern_ = wildcard; }
};

#endif /* SYS_FILE_H_ */
