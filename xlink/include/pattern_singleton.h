/*
 * pattern_singleton.h
 *
 *  Created on: Oct 8, 2009
 *      Author: monfee
 */

#ifndef PATTERN_SINGLETON_H_
#define PATTERN_SINGLETON_H_

template <typename T>
class pattern_singleton
{
//protected:
//	static T inst;

public:
	inline static T& instance() {
		static T inst;
		return inst;
	}
};

#endif /* PATTERN_SINGLETON_H_ */
