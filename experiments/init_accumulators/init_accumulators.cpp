/*
 INIT_ARRAY.C
 ------------
 test the time it takes to initialise the postings array against alternative techniques
 */

#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <sched.h>
#include "../../source/memory.h"
#include "../../source/maths.h"
#include "../../source/search_engine_init_flags_boolean.h"
#include "mt64.h"

//http://kerneltrap.org/node/4705
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

const long BITS_IN_WIDTH = 10;
const long DOCUMENTS = 3000000;
long postings_list[DOCUMENTS];

/*********************************************************************************
 *
 *
 *
 *
 ********************************************************************************/
class accumulator {
public:
	long documents;

public:
	accumulator(long documents) {
		this->documents = documents;
	}
	virtual ~accumulator() {
	}
	virtual void init(void) = 0;
	virtual void touch(long *what, long length) = 0;
	virtual void text_render(void) = 0;
};


/*********************************************************************************
 *
 *
 *
 *
 ********************************************************************************/
class accumulator_in_array: public accumulator {
public:
	short *buffer;

public:
	accumulator_in_array(long documents) : accumulator(documents) {
		buffer = new short[documents * 2];
		//buffer = new short[documents];
	} // * 2 for overflow reasons (delete when this is properly coded)
	virtual ~accumulator_in_array() {
		delete[] buffer;
	}
	virtual void init(void);
	virtual void touch(long *postings_list, long length);
	virtual void text_render(void);
};

void accumulator_in_array::init(void) {
	memset(buffer, 0, documents * sizeof(*buffer));
}

void accumulator_in_array::touch(long *postings_list, long length) {
	long *current, *end;

	end = postings_list + length;
	for (current = postings_list; current < end; current++) {
		buffer[*current]++;
	}
}

void accumulator_in_array::text_render(void) {
	long index;

	for (index = 0; index < documents; index++)
		if (buffer[index] != 0)
			printf("(%ld,%d) ", index, (int) buffer[index]);

	printf("\n");
}

/*********************************************************************************
 *
 *
 *
 *
 ********************************************************************************/
class accumulator_in_2D_POW2: public accumulator_in_array {
public:
#ifdef USE_BITSTRING
	ANT_search_engine_init_flags_boolean init_flags;
#else
	unsigned char *init_flags;
#endif

	long long width, height;
	short width_in_bits;

public:
	accumulator_in_2D_POW2(long documents, short width_bits);
	virtual ~accumulator_in_2D_POW2() {
#ifdef USE_BITSTRING

#else
		delete[] init_flags;
#endif
	}
	virtual void init(void);
	virtual void touch(long *postings_list, long length);
	virtual void text_render(void);
};

accumulator_in_2D_POW2::accumulator_in_2D_POW2(long documents, short width_bits) : accumulator_in_array(documents) {
	width_in_bits = width_bits;
	this->width = ANT_pow2_zero(width_in_bits);
	this->height = (documents / this->width) + 1;
#ifdef USE_BITSTRING

#else
	init_flags = new unsigned char[this->height];
#endif

	//reset accumulators to non-zero values
	memset(buffer, 0xCA, documents * sizeof(*buffer));
}

void accumulator_in_2D_POW2::init(void) {
#ifdef USE_BITSTRING
	init_flags.init(this->documents);
	init_flags.set_length(this->height);
#else
	memset(init_flags, 0, sizeof(*init_flags) * height);
#endif
}

void accumulator_in_2D_POW2::touch(long *postings_list, long length) {
	long *current, *end, row;

	end = postings_list + length;
	for (current = postings_list; current < end; current++) {
		//if (unlikely(init_flags[row = (*current >> width_in_bits)] == 0)) {
#ifdef USE_BITSTRING
		if (init_flags.get(row = (*current >> width_in_bits))) {
			init_flags.set(row);
#else
		if (init_flags[row = (*current >> width_in_bits)] == 0) {
			init_flags[row] = 1;
#endif
			memset(buffer + (row * width), 0, width * sizeof(*buffer));
		}
		buffer[*current]++;
	}
}

void accumulator_in_2D_POW2::text_render(void) {
	long index;

	for (index = 0; index < documents; index++)
#ifdef USE_BITSTRING
		if (init_flags.get(index >> width_in_bits)) {
#else
		if (init_flags[index >> width_in_bits] != 0) {
#endif
			if (buffer[index] != 0)
				printf("(%d,%d) ", (int) index, (int) buffer[index]);
		}
	printf("\n");
}


/*********************************************************************************
 *
 *
 *
 *
 ********************************************************************************/
class accumulator_in_2D_VARIABLE: public accumulator_in_array {
public:
#ifdef USE_BITSTRING
	ANT_search_engine_init_flags_boolean init_flags;
#else
	unsigned char *init_flags;
#endif
	long long width, height;

#ifdef VARIABLE_ROUNDED_TO_BEST
	long width_in_bits;
#endif

public:
	accumulator_in_2D_VARIABLE(long documents);
	virtual ~accumulator_in_2D_VARIABLE() {
#ifdef USE_BITSTRING

#else
		delete[] init_flags;
#endif
	}
	virtual void init(void);
	virtual void touch(long *postings_list, long length);
	virtual void text_render(void);
	void set_width(long new_width);
	long get_width(void);
};

accumulator_in_2D_VARIABLE::accumulator_in_2D_VARIABLE(long documents) : accumulator_in_array(documents) {
#ifdef USE_BITSTRING

#else
	init_flags = new unsigned char[documents];
#endif

	//reset accumulators to non-zero values
	memset(buffer, 0xCA, documents * sizeof(*buffer));
}

void accumulator_in_2D_VARIABLE::set_width(long new_width) {
	width = new_width;

	if (width == 0) {
		width = 1;
	}

#ifdef VARIABLE_ROUNDED_TO_BEST
	width_in_bits = (long)log2(width);
	if (abs(ANT_pow2_zero(width_in_bits+1) - width) < abs(ANT_pow2_zero(width_in_bits) - width))	 {
		width_in_bits++;
	}
	width = ANT_pow2_zero(width_in_bits);
	//printf("new_width: %ld, bits: %ld, width: %lld\n", new_width, width_in_bits, width);
#endif

	if (width == 1) {
		height = documents;
	} else {
		height = (documents / width) + 1;
	}
}

long accumulator_in_2D_VARIABLE::get_width(void) {
	return width;
}

void accumulator_in_2D_VARIABLE::init(void) {
#ifdef USE_BITSTRING
	init_flags.init(this->documents);
	init_flags.set_length(this->height);
#else
	memset(init_flags, 0, sizeof(*init_flags) * height);
#endif
}

void accumulator_in_2D_VARIABLE::touch(long *postings_list, long length) {
	long *current, *end, row;

	end = postings_list + length;
	for (current = postings_list; current < end; current++) {
		//if (unlikely(init_flags[row = (*current >> width_in_bits)] == 0)) {
#ifdef USE_BITSTRING
	#ifdef VARIABLE_ROUNDED_TO_BEST
		if (init_flags.get(row = (*current >> width_in_bits))) {
	#else
		if (init_flags.get(row = (*current / width))) {
	#endif
			init_flags.set(row);
#else
	#ifdef VARIABLE_ROUNDED_TO_BEST
		if (init_flags[row = (*current >> width)] == 0) {
	#else
		if (init_flags[row = (*current / width)] == 0) {
	#endif
			init_flags[row] = 1;
#endif
			memset(buffer + (row * width), 0, width * sizeof(*buffer));
		}
		buffer[*current]++;
	}
}

void accumulator_in_2D_VARIABLE::text_render(void) {
	long index;

	for (index = 0; index < documents; index++)
#ifdef USE_BITSTRING
	#ifdef VARIABLE_ROUNDED_TO_BEST
		if (init_flags.get(index >> width_in_bits)) {
	#else
		if (init_flags.get(index / width)) {
	#endif
#else
	#ifdef VARIABLE_ROUNDED_TO_BEST
		if (init_flags[index >> width] != 0) {
	#else
		if (init_flags[index / width] != 0) {
	#endif
#endif
			if (buffer[index] != 0)
				printf("(%d,%d) ", (int) index, (int) buffer[index]);
		}
	printf("\n");
}


/*
 GET_RANDOM_LIST()
 -----------------
 */
void get_random_list(long *into, long length, long max_value, long seed) {
	long current;
	init_genrand64(seed);
	for (current = 0; current < length; current++){
		into[current] = genrand64_int64() % max_value;
	}

}

/*
 * return the time difference in microseconds
 */
unsigned long long get_time_difference_in_microseconds(struct timeval *start, struct timeval *end) {
	return ((end->tv_sec * 1e6) + end->tv_usec) - ((start->tv_sec * 1e6) + start->tv_usec);
}

/*
 MAIN()
 ------
 */
int main(int argc, char* argv[]) {
	accumulator *array;
	accumulator_in_2D_POW2 *two_dimensional_pow2;
	accumulator_in_2D_VARIABLE *two_dimensional_variable;
	long bits_in_width = BITS_IN_WIDTH;
	long long width_in_variable = 0;
	long postings_list_length = 1000;
	long t, terms = 1000;
	long long timer;
	unsigned long long init_time_array = 0, init_time_2D_pow2 = 0, init_time_2D_variable = 0;
	unsigned long long touch_time_array = 0, touch_time_2D_pow = 0, touch_time_2D_variable = 0;
	struct timeval t1, t2;
	long global_seed = 0;
	long repeats = 1, r;
	int test_optimal_width = 0;

#ifdef __linux__
	cpu_set_t set;
	int ret = 0;

	CPU_ZERO(&set);
	CPU_SET(1, &set);
	ret = sched_setaffinity(0, sizeof(cpu_set_t), &set);
	if (ret < 0) {
		perror("sched_setaffinity\n");
		exit(3);
	}
#endif

	int i = 1;
	while ( i < argc) {
		if (strcmp(argv[i], "--width_in_bits") == 0) {
			bits_in_width = atol(argv[++i]);
			i++;
		} else if (strcmp(argv[i], "--postings_list_length") == 0) {
			postings_list_length = atol(argv[++i]);
			i++;
		} else if (strcmp(argv[i], "--num_of_terms") == 0) {
			terms = atol(argv[++i]);
			i++;
		} else if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
			printf("Usage: %s [-h] [--help] [--postings_list_length <length>] [--width_in_bits <bits>] [--num_of_terms <terms>] --seed <num> --optimal_width\n", argv[0]);
			exit(0);
		} else if ((strcmp(argv[i], "--seed") == 0)) {
			global_seed = atol(argv[++i]);
			i++;
		} else if (strcmp(argv[i], "--repeats") == 0) {
			repeats = atol(argv[++i]);
			i++;
		} else if (strcmp(argv[i], "--optimal_width") == 0) {
			test_optimal_width = 1;
			i++;
		} else {
			printf("'%s' is not supported\n", argv[i]);
			exit(0);
		}
	}



	array = new accumulator_in_array(DOCUMENTS);
	two_dimensional_pow2 = new accumulator_in_2D_POW2(DOCUMENTS, bits_in_width);
	if (test_optimal_width) {
		two_dimensional_variable = new accumulator_in_2D_VARIABLE(DOCUMENTS);
		width_in_variable = (long long)sqrt(128 * (DOCUMENTS / postings_list_length * terms));
		two_dimensional_variable->set_width(width_in_variable);
	}

	printf("       width_in_bits: %ld\n", bits_in_width);
	if (test_optimal_width) {
		printf("   width_in_variable: %ld\n", two_dimensional_variable->get_width());
	}
	printf("postings_list_length: %ld\n", postings_list_length);
	printf("        num_of_terms: %ld\n", terms);
	printf("                seed: %ld\n", global_seed);
	printf("             repeats: %ld\n", repeats);


	for (r = 0; r < repeats; r++) {
		gettimeofday(&t1, NULL);
		array->init();
		gettimeofday(&t2, NULL);
		init_time_array += get_time_difference_in_microseconds(&t1, &t2);
	}

	for (r = 0; r < repeats; r++) {
		gettimeofday(&t1, NULL);
		two_dimensional_pow2->init();
		gettimeofday(&t2, NULL);
		init_time_2D_pow2 += get_time_difference_in_microseconds(&t1, &t2);
	}

	if (test_optimal_width) {
		for (r = 0; r < repeats; r++) {
			gettimeofday(&t1, NULL);
			two_dimensional_variable->init();
			gettimeofday(&t2, NULL);
			init_time_2D_variable += get_time_difference_in_microseconds(&t1, &t2);
		}
	}

	for (t = 0; t < terms; t++) {
		get_random_list(postings_list, postings_list_length, DOCUMENTS, time(NULL)+global_seed);

		for (r = 0; r < repeats; r++) {
			gettimeofday(&t1, NULL);
			array->touch(postings_list, postings_list_length);
			gettimeofday(&t2, NULL);
			touch_time_array += get_time_difference_in_microseconds(&t1, &t2);
		}

		for (r = 0; r < repeats; r++) {
			gettimeofday(&t1, NULL);
			two_dimensional_pow2->touch(postings_list, postings_list_length);
			gettimeofday(&t2, NULL);
			touch_time_2D_pow += get_time_difference_in_microseconds(&t1, &t2);
		}

		if (test_optimal_width) {
			for (r = 0; r < repeats; r++) {
				gettimeofday(&t1, NULL);
				two_dimensional_variable->touch(postings_list, postings_list_length);
				gettimeofday(&t2, NULL);
				touch_time_2D_variable += get_time_difference_in_microseconds(&t1, &t2);
			}
		}

	}

	printf("      Array: %.3f milliseconds\n", (double)init_time_array / (1000 * repeats) + (double)touch_time_array / (1000 * repeats));
	printf("    2D_POW2: %.3f milliseconds\n", (double)init_time_2D_pow2 / (1000 * repeats)  + (double)touch_time_2D_pow / (1000 * repeats));
	if (test_optimal_width) {
		printf("2D_VARIABLE: %.3f milliseconds\n", (double)init_time_2D_variable / (1000 * repeats)  + (double)touch_time_2D_variable / (1000 * repeats));
	}

	return 0;
}
