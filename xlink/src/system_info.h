/*
 * system_info.h
 *
 *  Created on: Aug 3, 2009
 *      Author: monfee
 */

#ifndef SYSTEM_INFO_H_
#define SYSTEM_INFO_H_

#include <string>
/*
 *
 */
class system_info {
public:
	std::string cpu_info;

public:
	system_info();
	virtual ~system_info();
};

#endif /* SYSTEM_INFO_H_ */
