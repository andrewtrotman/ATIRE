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
#include "word.h"

class File {
public:
	typedef SEGMENTATION::Word::array_type			array_type;
	typedef SEGMENTATION::Word::word_type			word_type;
	typedef SEGMENTATION::Word::word_ptr_type		word_ptr_type;

public:
	std::string EXT_NAME;

protected:
	std::fstream 			iofs_;
	std::string 			name_;
	std::string				filename_;
	std::string				path_;
	std::fstream::pos_type	size_;

public:
	File() {}
	File(std::string name);
	File(std::string name, unsigned int size);
	virtual ~File();

	void initialize(unsigned int size);
	bool exist();
	const std::string& fullpathname() const { return filename_; }

	const std::string& name() const { return name_; }
	void name(std::string name) { name_ = name; }

	void path(std::string path) { path_ = path; filename_ = path_ + name_ + "." + EXT_NAME; }
	const std::string& path() const { return path_; }

protected:
	bool ropen();
	void wopen();
	void setup();
};

//std::string File::EXT_NAME;


#endif /*FILE_H_*/
