/*
 * template_algo.h
 *
 *  Created on: Oct 13, 2009
 *      Author: monfee
 */

#ifndef TEMPLATE_ALGO_H_
#define TEMPLATE_ALGO_H_

#include <algorithm>
#include <iterator>

template <typename T>
inline void bubble_sort(T& jar) {
	typedef typename T::value_type value_type;

	if (jar.size() <= 1)
		return;

	bool swapped = false;
	typename T::value_type ptr = NULL;
	do {
		typename T::iterator pre = jar.begin();
		typename T::iterator next = pre;
		++next;

	    swapped = false;
	    while (next != jar.end()) {
			if (!(*pre > *next)) {
				//swap(jar[i], jar[i+1]);
				ptr = *pre;
				*pre = *next;
				*next = ptr;
//				std::iter_swap(&pre, &next);
				swapped = true;
			}
			pre = next;
			++next;
	    }
	} while (swapped);

}

template <typename T>
inline void bubble_sort1(T& jar) {
	bool swapped = false;
	typename T::value_type ptr = NULL;
	do {
	    swapped = false;
	    for (int i = 0; i < (jar.size() - 1); i++) {
			if (!(*jar[i] > *jar[i+1])) {
				ptr = jar[i];
				jar[i] = jar[i + 1];
				jar[i + 1] = ptr;
				//swap(jar[i], jar[i+1]);

				swapped = true;
			}
	    }
	} while (swapped);

}

#endif /* TEMPLATE_ALGO_H_ */
