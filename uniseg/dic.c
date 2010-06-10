/*
 * dic.cpp
 *
 *  Created on: Feb 14, 2009
 *      Author: monfee
 */

#include "dic.h"
#include <string.h>

#include <fstream>
#include <iostream>
#include <functional>
#include <algorithm>

using namespace std;

template<typename _Tp>
struct partial_equal_to : public binary_function<_Tp, _Tp, bool>
{
  bool
  operator()(const _Tp& __x, const _Tp& __y) const
  { return __x == __y; }
};

template<>
struct partial_equal_to<pair<string, long> > : public binary_function<pair<string, long>, pair<string, long>, bool>
{
  bool
  operator()(const pair<string, long>& __x, const pair<string, long>& __y) const
  {
	  if (__x.first.length() > __y.first.length())
		  return false;
	  return __y.first.substr(0, __x.first.length()) == __x.first;
  }
};

template<typename _Tp>
struct partial_equal_to_right : public binary_function<_Tp, _Tp, bool>
{
  bool
  operator()(const _Tp& __x, const _Tp& __y) const
  { return __x == __y; }
};

template<>
struct partial_equal_to_right<pair<string, long> > : public binary_function<pair<string, long>, pair<string, long>, bool>
{
  bool
  operator()(const pair<string, long>& __x, const pair<string, long>& __y) const
  {
	  if (__x.first.length() > __y.first.length())
		  return false;
	  return __y.first.substr(__y.first.length() - __x.first.length(), __x.first.length()) == __x.first;
  }
};

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

bool Dic::fuzzy_search(const string_type word)
{
//	typedef std::equal_to<word_map::value_type> EqualTo;
//	const std::binder1st<EqualTo> equal_to_this =
//	        std::bind1st (EqualTo (), word);
	pair<string, long> key_pair = make_pair(word, 1);
	return std::find_if(list_.begin(), list_.end(), bind1st(partial_equal_to<pair<string, long> >(), key_pair)) != list_.end();
}

bool Dic::fuzzy_search_right(const string_type word)
{
	pair<string, long> key_pair = make_pair(word, 1);
	return std::find_if(list_.begin(), list_.end(), bind1st(partial_equal_to_right<pair<string, long> >(), key_pair)) != list_.end();
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

void Dic::load(string_type filename, long lang)
{
	lang_ = lang;
	load(filename);
}

void Dic::load(string_type filename)
{
	  string line;
	  string::size_type pos;
	  long freq = 1;

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

	      add(line.substr(0, pos), freq);
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
