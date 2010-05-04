/*
 * dic.cpp
 *
 *  Created on: Feb 14, 2009
 *      Author: monfee
 */

#include "dic.h"
#include <fstream>
#include <iostream>

using namespace std;

void Dic::add(string_type word, long freq) {
	if (word.length() <= 0)
		return;

	word_map::iterator it = list_.find(word);
	if (it == list_.end()) {
		list_.insert(std::make_pair(word, freq));
		update_stat(get_utf8_string_length(word), 1);
	}
	else
		it->second += freq;
}

bool Dic::find(string_type word) {
	return list_.find(word) != list_.end();
}

void Dic::save(string_type filename) {
	std::ofstream 			ofs;
	ostream* os;

	if (filename.length() > 0) {
		ofs.open(filename.c_str(), ios::binary | ios::trunc);
		os = &ofs;
	}
	else
		os = &cerr;

	const Dic::word_map& list = list_; //dic_.list();
	Dic::word_map::const_iterator iter;
    for (iter=list.begin(); iter != list.end(); ++iter)
    	//if (iter->second == lang)
    	(*os) << iter->first << " " << iter->second << endl;

    if (ofs.is_open())
    	ofs.close();
}

void Dic::load(string_type filename, long lang) {
	  string line;
	  string::size_type pos;
	  long freq = 1;
	  if (lang_ != lang)
		  lang_ = lang;

	  ifstream dicfile(filename.c_str());
	  if (dicfile.is_open()) {
	    while (! dicfile.eof() ) {
	      getline (dicfile,line);
	      pos = line.find(" ");
	      if (pos != string::npos) {
			  const char *start = line.c_str();
			  start += pos;
			  while (isspace(*start))
				  ++start;
			  freq = atol(start);
	      }

	      if (freq <= 0)
	    	  freq = 1;

	      add(line.substr(0, pos + 1), freq);
	    }
	    dicfile.close();
	  }
	  else
		  cerr << "Unable to open " << filename << endl;

}

void Dic::update_stat(int num, int howmany) {
	stat_map::iterator it = stat_.find(num);
	if (it != stat_.end())
		it->second += howmany;
	else {
		if (howmany > 0)
			stat_.insert(make_pair(num, howmany));
	}
}

int Dic::find_stat(int num) {
	stat_map::iterator it = stat_.find(num);
	return (it != stat_.end()) ? it->second : 0;
}

void Dic::show() {
	stat_map::const_iterator it = stat_.begin();
	for (; it != stat_.end(); it++) {
		word_map::const_iterator iner_it = list_.begin();
		for (; iner_it != list_.end(); iner_it++) {
			string_type word = iner_it->first;
			if (get_utf8_string_length(word) == it->first)
				cerr << iner_it->first << endl;
		}
	}
}
