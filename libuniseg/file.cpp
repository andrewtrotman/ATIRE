/*
 * file.cpp
 *
 *  Created on: Oct 22, 2008
 *      Author: monfee
 */

#include "file.h"
#include "qconf.h"
#include <iostream>
#include <string>
#include <utils/fs.h>

using namespace std;

File::File(std::string name, unsigned int size) :
	name_(name), size_(size){
	//open(name);
	setup();
}

File::File(std::string name) :
	name_(name), size_(0) {
	//open(name);
	setup();
}

File::~File() {
}

void File::setup() {
	//name_.append(EXT_NAME);
	//string path = string(QConf::instance().home()); //string(&FILESYSTEM::File<std::string>::SEPARATOR);
	//cout << "path: " << path << endl;
	//path.push_back(FILESYSTEM::File<std::string>::SEPARATOR);
	filename_ = name_ + EXT_NAME;
	// debug
	//cout << "setup filename " << filename_ << " "<< name_ << " " << EXT_NAME << endl;
}

void File::wopen() {
	iofs_.open(filename_.c_str(), ios::out | ios::binary | ios::trunc);
	if (size_ > 0)
		initialize(size_);
}

bool File::ropen() {
	if (!exist()) {
		cerr <<  filename_ << ": No such file" << endl;
		return false;
	}

	iofs_.open(filename_.c_str(), ios::in | ios::binary | ios::ate);
	size_ = iofs_.tellg();
	iofs_.seekg(0, ios::beg);
	return true;
}

void File::initialize(unsigned int size) {
	if (iofs_.is_open()) {
		iofs_.seekp(0, ios::beg);
		char c = 0;
		cout << "initializing file and set size " << size << endl;
		for (unsigned int i = 0; i < size; i++)
			iofs_.write((char* )&c, 1);
	}
}

bool File::exist() {
	FILESYSTEM::File<std::string> file(filename_);
	return file.exist();
}


