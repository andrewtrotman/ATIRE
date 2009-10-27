/*
 * sys_files.h
 *
 *  Created on: Sep 30, 2009
 *      Author: monfee
 */

#ifndef SYS_FILES_H_
#define SYS_FILES_H_

#include "sys_file.h"
#include <vector>
#include <string>

/*
 * This is a class for reading system files recursively
 */
class sys_files: public sys_file
{
private:
	std::vector<std::string>			list_;
	//std::vector<std::string>::iterator	cur_file_;

public:
	sys_files(const char *name);
	sys_files();
	virtual ~sys_files();

	const char *first();
	const char *next() ;
	const char *last() ;
	virtual void list(const char *name);
};

#endif /* SYS_FILES_H_ */
