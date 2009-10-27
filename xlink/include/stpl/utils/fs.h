#ifndef FS_H_
#define FS_H_

#include <cerrno>
#include <vector>
#include <string>
#include <iostream>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace FILESYSTEM {

	template <typename StringT = std::string >
	class File {
	private:
		StringT name_;

	public:
		static const char SEPARATOR;

	public:
		//File() {}
		File(StringT name) : name_(name) {}
		~File() {}

		int list(std::vector<StringT> &files, bool recursive = false) {
			return list(name_, files, recursive);
		}

		StringT name() { return name_; }

		static int list(StringT dir, std::vector<StringT> &files, bool recursive = false)
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
		    	StringT in_dir = dir;
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

		bool exist() {
			bool success = true;
			struct stat my_stat;

			if( stat( name_.c_str(), &my_stat ) == -1 )
				success = false;

			return success;
		}

		static char* separator() { return &SEPARATOR; }
	};

	typedef File<std::string>	SFile;

	template <typename StringT>
#ifdef unix
	const char File<StringT>::SEPARATOR = '/';
#elif _WIN32
	const char File<StringT>::SEPARATOR = '\\';
#else
	const char File<StringT>::SEPARATOR = '/';
#endif

}
#endif /*FS_H_*/
