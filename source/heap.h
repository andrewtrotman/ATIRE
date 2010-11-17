#ifndef HEAP_H
#define HEAP_H

#include <math.h>
#include "primary_cmp.h"
#include "search_engine_accumulator.h"

template <typename T, typename _Compare = Primary_cmp<T> > class Heap {
private:
	T *array;
	long long size;
	inline void swap (T *a, T *b) {
		T temp = *a;
		*a = *b;
		*b = temp;
	}
	_Compare compare;

public:
	Heap(T &arry, long long size, const _Compare& __c = _Compare()) : array(&arry), size(size), compare(__c) {};

	#define parent_pos(i) (((i)-1) >> 1)
	#define left_pos(i) (((i) << 1) + 1)
	#define right_pos(i) (((i) << 1) + 2)

	void set_size(long long size) { this->size = size; }

	void max_heapify(long long pos);
	void max_heapify(long long pos, long long hsize);
	void build_max_heap(void);
	void max_heapsort(void);
	void print(long long i);

	void min_heapify(long long pos);
	void min_heapify(long long pos, long long hsize);
	void build_min_heap(void);
	void min_heapsort(void);
	int min_insert(T key, long *positions, ANT_search_engine_accumulator *base);
	//void min_update(T key);
	void min_update(long *positions, long index, ANT_search_engine_accumulator *base);
};

template <typename T, typename _Compare> int Heap<T, _Compare>::min_insert(T key, long *positions, ANT_search_engine_accumulator *base) {
	long long i = 0, lpos, rpos;
	// if key is less than the minimum in heap, then do nothing
	if (compare(key, array[0]) < 0) {
		return 0;
	}

	while (i < this->size) {
		lpos = left_pos(i);
		rpos = right_pos(i);

		// check array out of bound, it's also the stopping condition
		if ((lpos >= this->size) || (rpos >= this->size)) {
			break;
		}

		//if ((key < array[lpos]) && (key < array[rpos])) {
		if ((compare(key, array[lpos]) < 0) && (compare(key, array[rpos]) < 0)) {
			break;
		//} else if (array[lpos] < array[rpos]) {
		} else if (compare(array[lpos], array[rpos]) < 0) {
			positions[array[lpos]-base] = i;
			array[i] = array[lpos];
			i = lpos;
		} else {
			positions[array[rpos]-base] = i;
			array[i] = array[rpos];
			i = rpos;
		}
	}

	positions[key-base] = i;
	array[i] = key;
	return 1;
}

//template <typename T, typename _Compare> void Heap<T, _Compare>::min_update(T key) {
template <typename T, typename _Compare> void Heap<T, _Compare>::min_update(long *positions, long index, ANT_search_engine_accumulator *base) {
	long long i, lpos, rpos;
	T key;
/*
	for (i = 0; i < this->size; i++) {
		if (array[i] == key) {
			break;
		}
	}
*/
	i = positions[index];
	key = array[i];

	while (i < this->size) {
		lpos = left_pos(i);
		rpos = right_pos(i);

		if ((lpos >= this->size) || (rpos >= this->size)) {
			break;
		}

		if ((compare(key, array[lpos]) <= 0) && (compare(key, array[rpos]) <= 0)) {
			break;
		}

		if (compare(array[lpos], array[rpos]) > 0) {
			positions[array[rpos]-base] = i;
			array[i] = array[rpos];
			i = rpos;
		} else {
			positions[array[lpos]-base] = i;
			array[i] = array[lpos];
			i = lpos;
		}
	}

	positions[key-base] = i;
	array[i] = key;
}

template <typename T, typename _Compare> void Heap<T, _Compare>::max_heapify(long long pos) {
	long long left = left_pos(pos);
	long long right = right_pos(pos);
	long long largest;

	//if ((left < size) && (array[left] > array[pos])) {
	if ((left < size) && (compare(array[left], array[pos]) > 0)) {
		largest = left;
	} else {
		largest = pos;
	}

	//if ((right < size) && (array[right] > array[largest])) {
	if ((right < size) && (compare(array[right], array[largest]) > 0)) {
		largest = right;
	}

	if (largest != pos) {
		swap(&array[pos], &array[largest]);
		max_heapify(largest);
	}
}

template <typename T, typename _Compare> void Heap<T, _Compare>::max_heapify(long long pos, long long hsize) {
	long long left = left_pos(pos);
	long long right = right_pos(pos);
	long long largest;

	//if ((left < hsize) && (array[left] > array[pos])) {
	if ((left < hsize) && (compare(array[left], array[pos]) > 0)) {
		largest = left;
	} else {
		largest = pos;
	}

	//if ((right < hsize) && (array[right] > array[largest])) {
	if ((right < hsize) && (compare(array[right], array[largest]) > 0)) {
		largest = right;
	}

	if (largest != pos) {
		swap(&array[largest], &array[pos]);
		max_heapify(largest, hsize);
	}
}

template <typename T, typename _Compare> void Heap<T, _Compare>::build_max_heap() {
	for (long long i = size/2-1; i >= 0; i--) {
		max_heapify(i);
	}
}

template <typename T, typename _Compare> void Heap<T, _Compare>::print(long long i) {
	printf("i: %lld, ", i);
	for (long long i = 0; i < size; i++) {
		printf("%lld ", array[i]);
	}
	printf("\n");
}

/*
 * Sort in ascending order
 */
template <typename T, typename _Compare> void Heap<T, _Compare>::max_heapsort() {
	build_max_heap();
	for (long long i = size-1; i >= 1; i--) {
		//print(i);
		swap(&array[0], &array[i]);
		//print(i);
		max_heapify(0, i);
		//print(i);
		//printf("\n");
	}
}



template <typename T, typename _Compare> void Heap<T, _Compare>::min_heapify(long long pos) {
	long long left = left_pos(pos);
	long long right = right_pos(pos);
	long long smallest;

	//if ((left < size) && (array[left] < array[pos])) {
	if ((left < size) && (compare(array[left], array[pos]) < 0)) {
		smallest = left;
	} else {
		smallest = pos;
	}

	//if ((right < size) && (array[right] < array[smallest])) {
	if ((right < size) && (compare(array[right], array[smallest]) < 0)) {
		smallest = right;
	}

	if (smallest != pos) {
		swap(&array[pos], &array[smallest]);
		min_heapify(smallest);
	}
}

template <typename T, typename _Compare> void Heap<T, _Compare>::min_heapify(long long pos, long long hsize) {
	long long left = left_pos(pos);
	long long right = right_pos(pos);
	long long smallest;

	//if ((left < hsize) && (array[left] < array[pos])) {
	if ((left < hsize) && (compare(array[left], array[pos])) < 0) {
		smallest = left;
	} else {
		smallest = pos;
	}

	//if ((right < hsize) && (array[right] < array[smallest])) {
	if ((right < hsize) && (compare(array[right], array[smallest]) < 0)) {
		smallest = right;
	}

	if (smallest != pos) {
		swap(&array[pos], &array[smallest]);
		min_heapify(smallest, hsize);
	}
}

template <typename T, typename _Compare> void Heap<T, _Compare>::build_min_heap() {
	for (long long i = size/2-1; i >= 0; i--) {
		min_heapify(i);
	}
}

/*
 * Sort in descending order
 */
template <typename T, typename _Compare> void Heap<T, _Compare>::min_heapsort() {
	build_min_heap();
	for (long long i = size-1; i >= 1; i--) {
		//print(i);
		swap(&array[0], &array[i]);
		//print(i);
		min_heapify(0, i);
		//print(i);
		//printf("\n");
	}
}

#endif
