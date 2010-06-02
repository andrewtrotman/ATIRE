/*
 * file.h
 *
 *  Created on: Oct 22, 2008
 *      Author: monfee
 */

#ifndef FILE_H_
#define FILE_H_

#include <string>
#include <fstream>
#include <vector>
#include "uniseg_types.h"

class File {

public:
	std::string EXT_NAME;
	static const char SEPARATOR;
	static const int INT_TYPE_SIZE = sizeof(unsigned int);

protected:
	std::fstream 			iofs_;
	std::string 			name_;
	std::string				filename_;
	std::string				path_;
	std::fstream::pos_type	size_;

	char 					*buf_;
	bool					in_memory_;
	uniseg_encoding 		*enc_;

public:
	File() {}
	File(std::string name);
	File(std::string name, unsigned int size);
	virtual ~File();

	void initialize(unsigned int size);
	bool exist();
	static bool exist(const char *filename);

	const std::string& fullpathname() const { return filename_; }
	void set_fullpathname(const char *name);

	const std::string& name() const { return name_; }
	void name(std::string name) { name_ = name; }

	void path(std::string path) { path_ = path; filename_ = path_ + name_ + "." + EXT_NAME; }
	const std::string& path() const { return path_; }

	static int list(std::string dir, std::vector<std::string> &files, bool recursive = false);
	//static char *read(const char *filename);
	void read_in_memory();

protected:
	bool ropen();
	void wopen();
	void setup();
};

//std::string File::EXT_NAME;


#endif /*FILE_H_*/
