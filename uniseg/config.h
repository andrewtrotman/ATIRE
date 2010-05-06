/*
 * config.h
 *
 *  Created on: May 6, 2010
 *      Author: monfee
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include <vector>

class Property
{
public:
	enum {COMMENT, PROPERTY};
	int 			property_type;
	std::string 	name;
	std::string		value;

	bool operator == (const Property& rhs) const {
		return name == rhs.name;
	}
};

class Config
{
private:
	std::vector<Property> properties_;

public:
	Config();
	virtual ~Config();

	bool load(const char *filename);
	std::string get_property(std::string name);
};

#endif /* CONFIG_H_ */
