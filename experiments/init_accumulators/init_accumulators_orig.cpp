/*
 INIT_ARRAY.C
 ------------
 test the time it takes to initialise the postings array against alternative techniques
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../source/memory.h"
#include "../source/stats.h"
#include "../source/maths.h"

const long BITS_IN_WIDTH = 10;
const long DOCUMENTS = 3000000;
long random_list[DOCUMENTS];

/*
 class VERSION
 -------------
 */
class version {
public:
	long documents;

public:
	version(long documents) {
		this->documents = documents;
	}
	virtual ~version() {
	}
	virtual void init(void) = 0;
	virtual void touch(long *what, long length) = 0;
	virtual void text_render(void) = 0;
};

/*
 class ARRAY_VERSION
 -------------------
 */
class array_version: public version {
public:
	short *buffer;

public:
	array_version(long documents) : version(documents) {
		buffer = new short[documents * 2];
	} // * 2 for overflow reasons (delete when this is properly coded)
	virtual ~array_version() {
		delete[] buffer;
	}
	virtual void init(void);
	virtual void touch(long *what, long length);
	virtual void text_render(void);
};

/*
 ARRAY_VERSION::INIT()
 ---------------------
 */
void array_version::init(void) {
	memset(buffer, 0, documents * sizeof(*buffer));
}

/*
 ARRAY_VERSION::TOUCH()
 ----------------------
 */
void array_version::touch(long *what, long length) {
	long *current, *end;

	end = what + length;
	for (current = what; current < end; current++)
		buffer[*current]++;
}

/*
 ARRAY_VERSION::TEXT_RENDER()
 ----------------------------
 */
void array_version::text_render(void) {
	long index;

	for (index = 0; index < documents; index++)
		if (buffer[index] != 0)
			printf("(%ld,%d) ", index, (int) buffer[index]);

	printf("\n");
}

/*
 class ONE_DIMENSIONAL_VERSION
 -----------------------------
 */
class one_dimensional_version: public array_version {
public:
	unsigned char *pointers;
	long width, height;
	short bits;

public:
	one_dimensional_version(long documents, short width_bits);
	virtual ~one_dimensional_version() {
		delete[] pointers;
	}
	virtual void init(void);
	virtual void touch(long *what, long length);
	virtual void text_render(void);
};

/*
 ONE_DIMENSIONAL_VERSION::ONE_DIMENSIONAL_VERSION()
 --------------------------------------------------
 */
one_dimensional_version::one_dimensional_version(long documents,
		short width_bits) : array_version(documents) {
	bits = width_bits;
	//this->width = ANT_pow2(bits);
	this->width = ANT_pow2_zero(bits);
	//printf("width: %ld\n", this->width);
	this->height = (documents / this->width) + 1;
	pointers = new unsigned char[this->height];

	memset(buffer, 0xCA, documents * sizeof(*buffer));
}

/*
 ONE_DIMENSIONAL_VERSION::INIT()
 -------------------------------
 */
void one_dimensional_version::init(void) {
	memset(pointers, 0, sizeof(*pointers) * height);
}

/*
 ONE_DIMENSIONAL_VERSION::TOUCH()
 --------------------------------
 */
void one_dimensional_version::touch(long *what, long length) {
	long *current, *end, row;

	end = what + length;
	for (current = what; current < end; current++) {
		if (pointers[row = (*current >> bits)] == 0) {
			pointers[row] = 1;
			memset(buffer + (row * width), 0, width * sizeof(*buffer));
		}
		buffer[*current]++;
	}
}

/*
 ONE_DIMENSIONAL_VERSION::TEXT_RENDER()
 --------------------------------------
 */
void one_dimensional_version::text_render(void) {
	long index;

	for (index = 0; index < documents; index++)
		if (pointers[index >> bits] != 0)
			if (buffer[index] != 0)
				printf("(%d,%d) ", (int) index, (int) buffer[index]);
	printf("\n");
}

/*
 GET_RANDOM_LIST()
 -----------------
 */
void get_random_list(long *into, long length, long max_value, long seed) {
	long current;

	srand(seed);
	for (current = 0; current < length; current++) {
		into[current] = (double) rand() / ((unsigned long long)RAND_MAX + 1) * max_value;
		//printf("rand: %f\n", (double) rand() / ((unsigned long long)RAND_MAX + 1) * max_value);
		//printf("rand: %ld\n", into[current]);
	}
}

/*
 MAIN()
 ------
 */
int main(void) {
	ANT_stats stats;
	version *array, *one_dimensional;
	long random_list_length = 1000;
	long which, times = 1000;
	long long timer;

	get_random_list(random_list, random_list_length, DOCUMENTS, 0);

	array = new array_version(DOCUMENTS);
	timer = stats.start_timer();
	for (which = 0; which < times; which++) {
		array->init();
		array->touch(random_list, random_list_length);
	}
	stats.print_time("Array: ", stats.stop_timer(timer));
	//array->text_render();

	one_dimensional = new one_dimensional_version(DOCUMENTS, BITS_IN_WIDTH);
	timer = stats.start_timer();
	for (which = 0; which < times; which++) {
		one_dimensional->init();
		one_dimensional->touch(random_list, random_list_length);
	}
	stats.print_time("Tree : ", stats.stop_timer(timer));
	//one_dimensional->text_render();

	return 0;
}
