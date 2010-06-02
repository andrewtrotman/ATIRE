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
#include <stdexcept>

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

void File::set_fullpathname(const char *name)
{
	filename_ = name;
	path_ = filename_.substr(0, filename_.rfind("/") + 1);
	name_ = filename_.substr(path_.length(), filename_.rfind(".") - path_.length());
}

void File::read_in_memory() {
	if (in_memory_)
		return;

	if (!iofs_.is_open())
		ropen();
	if (iofs_.is_open()) {
		buf_ = new char [static_cast<int>(size_) + 1];
		iofs_.read (buf_, size_);
		buf_[size_] = '\0';
		in_memory_ = true;
	}
	/**
	 * TODO throw an exception when can't open the file
	 */
	else  throw std::runtime_error(filename_ + ": Unable to open file");
}

void File::setup() {
	//name_.append(EXT_NAME);
	//string path = string(UNISEG_settings::instance().home()); //string(&FILESYSTEM::File<std::string>::SEPARATOR);
	//cerr << "path: " << path << endl;
	//path.push_back(FILESYSTEM::File<std::string>::SEPARATOR);
//	if (filename_.length() == 0)
//		filename_ = name_ + "." + EXT_NAME;
	// debug
	//cerr << "setup filename " << filename_ << " "<< name_ << " " << EXT_NAME << endl;
	in_memory_ = false;
	enc_ = uniseg_encoding_factory::instance().get_encoding();
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
		cerr << "initializing file and set size " << size << endl;
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
       // cerr << "Error(" << errno << ") opening " << dir << endl;
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
	exist(filename_.c_str());
}

bool File::exist(const char *filename)
{
	bool success = true;
	struct stat my_stat;

	if( stat( filename, &my_stat ) == -1 )
		success = false;

	return success;
}

