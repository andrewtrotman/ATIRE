/*
	TOP_K_SORT.H
	------------
*/

#ifndef TOP_K_SORT_H_
#define TOP_K_SORT_H_

#include <stddef.h>	// needed for size_t

void top_k_sort(void *base, size_t num, size_t width, int (*comp)(const void *, const void *));

#endif  /* TOP_K_SORT_H_ */

