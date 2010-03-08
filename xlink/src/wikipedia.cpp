/*
 * wikipedia.cpp
 *
 *  Created on: Oct 7, 2009
 *      Author: monfee
 */

#include "wikipedia.h"
#include "string_utils.h"

#include <iostream>

using namespace QLINK;
using namespace std;

wikipedia::wikipedia()
{

}

wikipedia::~wikipedia()
{
}

std::pair<std::string, std::string> wikipedia::process_title(std::string& orig, bool lowercase)
{
	std::string title("");
	std::string desc("");

//	if (orig.find("Homer Simpson") != string::npos)
//		cerr << "I got you" << endl;

	string::iterator it = orig.begin();
	while ((*it) == ' ')
		++it;

	for (; it != orig.end(); it++) {
		if ((*it) == '&') {
			title.push_back(' ');
			++it;
			while ((*it) != ';')
				it++;
			++it;

			if (it == orig.end())
				break;
		}
		if ((*it) == '(') {
			++it;
			while (it != orig.end() && (*it) != ')') {
				desc.push_back(*it);
				++it;
			}
			break;
		}

		if (!isalnum(*it))
			*it = ' ';
		title.push_back(lowercase ? tolower(*it) : *it);
	}
	string::size_type pos = title.find_last_not_of(" ");
	title.erase(pos + 1);
	trim_left(desc, " ");
	trim_right(desc, "");
	return make_pair(title, desc);
}
