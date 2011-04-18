/*
 * translation.cpp
 *
 *  Created on: Apr 18, 2011
 *      Author: monfee
 */

#include "translation.h"
#include "sys_file.h"
#include "run_config.h"
#include "google_translator.h"

#include <fstream>
#include <utility>
#include <sstream>
#include <iostream>

using namespace QLINK;
using namespace std;

const char *translation::CACHE_FOLDER = ".translation_cache";

translation::translation()
{
	if (sys_file::exist(CACHE_FOLDER)) {
		ifstream myfile (CACHE_FOLDER);

		if (myfile.is_open()) {
			while (! myfile.eof()) {
				string line;
				getline (myfile, line);
				string::size_type pos = line.find_first_of(':');
				if (pos != string::npos) {
					translations_[line.substr(0, pos)] = line.substr(pos);
				}
			}
		}
	}

	if (run_config::instance().get_value("create_translation_cache").length() > 0)
		cache_it_ = true;
}

translation::~translation()
{
	if (cache_it_) {
		ofstream out(CACHE_FOLDER);
		map<std::string, std::string>::iterator it = translations_.begin();
		while (it != translations_.end()) {
			out << it->first << ":" << it->second << endl;
			++it;
		}
		out.close();
	}
}


std::string translation::translate(const char *word, const char *language_pair)
{

	map<std::string, std::string>::iterator it = translations_.find(word);
	if (it != translations_.end())
		return it->second;

	std::string result = google_translator::instance().translate(word, language_pair);
	translations_[word] = result;

	return result;
}
