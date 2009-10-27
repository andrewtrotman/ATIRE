/*
 * singleton.h
 *
 *  Created on: Oct 8, 2009
 *      Author: monfee
 */

#ifndef SINGLETON_H_
#define SINGLETON_H_

template <typename T>
class pattern_singleton
{
public:
	inline static T& instance() {
		static T inst;
		return inst;
	}
};

#endif /* SINGLETON_H_ */
