/*
 * file.cpp
 *
 *  Created on: Oct 22, 2008
 *      Author: monfee
 */

#include "file.h"
#include "uniseg_settings.h"
#include <iostream>
#include <string>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

#ifdef unix
	const char File::SEPARATOR = '/';
#elif defined(_WIN32) || defined(_MSC_VER) || defined(_WIN64)
	const char File::SEPARATOR = '\\';
#else
	const char File::SEPARATOR = '/';
#endif

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
	//string path = string(UNISEG_settings::instance().home()); //string(&FILESYSTEM::File<std::string>::SEPARATOR);
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

int File::list(std::string dir, std::vector<std::string> &files, bool recursive)
{
    DIR *dp;
    struct dirent *dirp;
    if(/*exist() || */(dp  = opendir(dir.c_str())) == NULL) {
    	// TODO throw a error exception
       // cout << "Error(" << errno << ") opening " << dir << endl;
    	return -1;
    }

    while ((dirp = readdir(dp)) != NULL) {
    	std::string temp_str(dirp->d_name);
    	if (temp_str == "." || temp_str == "..")
    		continue;

    	// TODO
    	std::string in_dir = dir;
    	in_dir.push_back(SEPARATOR);
    	in_dir.append(dirp->d_name);
    	if( dirp->d_type == DT_DIR && recursive) {
    		chdir(in_dir.c_str());
    	    list(in_dir, files, recursive);
    	    chdir( ".." );
    	} else
    		files.push_back(in_dir);
    }
    closedir(dp);
    return files.size();
}

bool File::exist() {
	bool success = true;
	struct stat my_stat;

	if( stat( name_.c_str(), &my_stat ) == -1 )
		success = false;

	return success;
}


