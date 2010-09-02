

#ifndef HEAP_H
#define HEAP_H

#include <math.h>
#include "primary_cmp.h"

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
//	inline long long parent_pos(long long i) { return (i-1)/2; }
//	inline long long left_pos(long long i) { return i*2+1; }
//	inline long long right_pos(long long i) { return i*2+2; }

	#define parent_pos(i) (((i)-1)/2)
	#define left_pos(i) ((i)*2+1)
	#define right_pos(i) ((i)*2+2)

	void max_heapify(long long pos);
	void max_heapify(long long pos, long long hsize);
	void build_max_heap(void);
	void max_heapsort(void);
	void print(long long i);

	void min_heapify(long long pos);
	void min_heapify(long long pos, long long hsize);
	void build_min_heap(void);
	void min_heapsort(void);
};

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
