/*
 * corpus.cpp
 *
 *  Created on: Sep 16, 2009
 *      Author: monfee
 */

#include "corpus.h"
#include "ant_link_parts.h"
#include "sys_files.h"

#include <string>
#include <fstream>
#include <utility>
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace QLINK;
using namespace std;

corpus::corpus() : doclist_("doclist.aspt")
{
	init();
}

corpus::corpus(const char *doclist) : doclist_(doclist)
{
	init();
}

corpus::~corpus()
{

}

void corpus::init()
{
	load_ = false;
}

void corpus::load()
{
	if (!load_)
		return;

	ifstream myfile (doclist_);

	string line;
	if (myfile.is_open()) {
		while (! myfile.eof()) {
		  getline (myfile, line);
		  string name(line, 0, line.find_last_of('.'));
		  if (line.length() > 0) {
			  unsigned long doc_id = result_to_id(line.c_str());
			  doc_map_.insert(make_pair(doc_id, name));
		  }
		  //cout << line << endl;
		}
		myfile.close();
	}
}

void corpus::load_teara_map()
{
	if (teara_home_.length() <= 0)
		return;

	sys_files disk;
	string wc = wildcard();
	disk.pattern(wc.c_str());

	if (!disk.isdir(teara_home_.c_str()))
		cerr << "TEARA HOME is not a directory!" << endl;
	const char *name = NULL;

	disk.list(teara_home_.c_str());
	for (name = disk.first(); name != NULL ; name = disk.next()) {
		char filename[1024];
		char *end = NULL;
		char namepart[1024];
		strcpy(filename, name);
		strcpy(namepart, basename(filename));
		end = strchr(namepart, '.');
		if (end != NULL)
			namepart[end - namepart] = '\0';
//		if (strncmp("Volcanic Plateau - Entry-9", namepart, strlen(namepart)) == 0)
//			cerr << "I caught you!" << endl;
		teara_map_.insert(make_pair(string(namepart), string(name)));
	}
}

std::string corpus::id2dir(unsigned long id)
{
	long last3 = id > 1000 ? (id - (id /1000) * 1000) : id;
	std::ostringstream stm;
	stm << std::setw(3) << std::setfill('0') << last3 << sys_file::SEPARATOR << id;
	return stm.str();
}

string corpus::id2doc(unsigned long id)
{
	doc_map::iterator iter = doc_map_.find(id);
	return iter != doc_map_.end() ? iter->second : "";
}

string corpus::id2docpath(unsigned long id)
{
//	if (load_)
//		return home_ + id2doc(id) + ".xml";
//
//	std::ostringstream stm;
//	stm << id;
//	string cmd("/home/monfee/workspace/qlink/src/scripts/doc2xmlpath.sh ");
//	cmd.append(stm.str());
//
//	return system_call::instance().execute(cmd.c_str());
	return home_ + id2dir(id) + ext();
}

string corpus::name2docpath(const char *name)
{
	name2docpath(home_, name);
}

string corpus::name2docpath(const string& home, const char *name)
{
	string sname(name);
	//string::size_type pos = name ;
	string part(sname, base_.size());
	string::size_type pos = part.find('.');
	if (pos != string::npos)
		part.erase(pos);
	return home + part + ext();
}

string corpus::name2tearapath(const char *name)
{
	teara_map::iterator iter = teara_map_.find(name);
	if (iter != teara_map_.end())
		return iter->second;
//	if (strncmp("Volcanic Plateau - Entry-9", name, strlen(name)) == 0)
//		cerr << "I caught you!" << endl;
//	teara_map::iterator iter = teara_map_.begin();
//	for (; iter != teara_map_.end(); ++iter) {
//		if (strcmp(iter->first.c_str(), name) == 0)
//			return iter->second;
//	}
	return ""; //name2docpath(teara_home_, name);
}
