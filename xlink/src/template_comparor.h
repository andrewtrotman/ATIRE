/*
 * template_comparor.h
 *
 *  Created on: Oct 13, 2009
 *      Author: monfee
 */

#ifndef TEMPLATE_COMPAROR_H_
#define TEMPLATE_COMPAROR_H_

#include <functional>

template <typename T>
struct pointer_less_than : public std::binary_function<bool, const T*, const T*> {
	const bool operator()(const T *a, const T * b) const {
		// check for 0
		if (a == 0)
			return b != 0; // if b is also 0, then they are equal, hence a is not < than b
		else if (b == 0)
			return false;
		else {
			const T& one = *a;
			const T& two = *b;
			return one < two;
		}
	}
};

template <typename T>
struct pointer_greater_than : public std::binary_function<bool, const T*, const T*> {
	const bool operator()(const T *a, const T * b) const {
		// check for 0
		if (a == 0)
			return b == 0; // if b is also 0, then they are equal, hence a is not > than b
		else if (b == 0)
			return true;
		else {
			const T& one = *a;
			const T& two = *b;
			return one > two;
		}
	}
};

#endif /* TEMPLATE_COMPAROR_H_ */
