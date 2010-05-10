/*
 * config.c
 *
 *  Created on: May 6, 2010
 *      Author: monfee
 */

#include "config.h"
#include <algorithm>
#include <fstream>

using namespace std;

Config::Config()
{

}

Config::~Config()
{
}

bool Config::load(const char *filename)
{
	string line;
	string::size_type pos;

	ifstream is(filename);

	if (is.is_open()) {
	  while (! is.eof() ) {
		getline (is,line);
		if (line.length() > 0) {
			properties_.push_back(Property());

			pos = line.find("=");
			if (pos != string::npos) {
				  const char *start = line.c_str();
				  start += pos;
				  ++start;
				  while (isspace(*start))
					  ++start;
				  properties_.back().value = string(start);
				  int len = properties_.back().value.length();
				  int pos1 = len - 1;
				  while (properties_.back().value[len] == ' ')
					  --len;
				  if (pos1 != (len -1))
					  properties_.back().value.erase(len);


					while (line[--pos] == ' ');
					++pos;
			}

			properties_.back().name = line.substr(0, pos);

			if (properties_.back().name.length() > 0 && properties_.back().name[0] == '#')
				properties_.back().property_type = Property::COMMENT;
		}
	  }
	  is.close();
	  return true;
	}
	return false;
}

std::string Config::get_property(std::string name)
{
	Property a_property;
	a_property.name = name;

	std::vector<Property>::iterator it = std::find_if(properties_.begin(), properties_.end(), std::bind2nd(std::equal_to<Property>(), a_property));

	if (it != properties_.end())
		return it->value;
	return "";
}
