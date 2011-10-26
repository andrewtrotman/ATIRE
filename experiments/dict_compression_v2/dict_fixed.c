#ifdef __linux__
	#ifndef _GNU_SOURCE
		#define _GNU_SOURCE
	#endif
	#ifndef __USE_GNU
		#define __USE_GNU
	#endif
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#if (USE_O_DIRECT == 1)
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
#endif


static FILE *dict_file = NULL;
static FILE *vocab_file = NULL;
static long long term_count = 0;
static dict_fixed_t *all_terms = NULL;
static long long node_max_size;

void build_fixed() {
	term_count = 0;
	dict_fixed_t one_term;
	long long i;

	node_max_size = hd_sector_size * num_of_sectors;
	printf("    node_max_size: %lld\n", node_max_size);
	printf("\nbuilding fixed......\n");

	//
	// (1) read term_count
	//
	vocab_file = fopen(VOCAB_FILENAME, "rb");
	dict_file = fopen(DICT_FIXED_FILENAME, "wb");
	fread(&term_count, sizeof(term_count), 1, vocab_file);
	dbg_printf("term_count: %lld\n", term_count);

	//
	// (2) read vocab and write to dict
	//
	for (i = 0; i < term_count; i++) {
		fread(one_term.term, MAX_WORD_LENGTH+1, 1, vocab_file);
		fwrite(&one_term, sizeof(one_term), 1, dict_file);
	}

#if (USE_O_DIRECT == 1)
	// make sure the last node is in sector size
	char one_byte = '\0';
	int padding = node_max_size - ftell(dict_file) % node_max_size;
	for (i = 0; i < padding; i++) {
		fwrite(&one_byte, 1, 1, dict_file);
	}
#endif

	printf("     total_wasted: %lld bytes\n", 0LL);
	fseek(dict_file, 0, SEEK_END);
	printf("  total file size: %ld\n", ftell(dict_file));
	fclose(dict_file);
	fclose(vocab_file);
	free(all_terms);
	printf("FINISHED\n\n");
}


void serialise_fixed() {
	FILE *serialised_file = NULL;
	dict_fixed_t one_term;

	printf("\nserialising fixed......");

	//
	// (1) open necessary files
	//
	dict_file = fopen(DICT_FIXED_FILENAME, "rb");
	if (!dict_file) { fprintf(stderr, "ERROR open file %s\n", DICT_FIXED_FILENAME); exit(2); }
	serialised_file = fopen(SERIALISE_FIXED_FILENAME, "wb");
	if (!serialised_file) { fprintf(stderr, "ERROR open file %s\n", SERIALISE_FIXED_FILENAME); exit(2); }

	//
	// (2) for testing purpose
	//
	dbg_printf("sizeof(dict_fixed_t): %lu\n", sizeof(dict_fixed_t));
	fseek(dict_file, 0, SEEK_END);
	term_count = ftell(dict_file) / sizeof(dict_fixed_t);
	dbg_printf("term_count: %lld\n", term_count);

	//
	// (3) start to serialise terms
	//
	fseek(dict_file, 0, SEEK_SET);
	while(fread(&one_term, sizeof(one_term), 1, dict_file)) {
		fwrite(one_term.term, strlen(one_term.term), 1, serialised_file);
		fwrite("\n", 1, 1, serialised_file);
	}


	fclose(serialised_file);
	fclose(dict_file);
	printf("\n");
}

static dict_fixed_header_t *headers_array;
static uint32_t node_count;
node_length_t node_length;
static dict_fixed_t *node_buffer;
#if (USE_O_DIRECT == 1)
	static int dict_file_odirect;
	static void *aligned_buffer;
#endif

void search_fixed_ondisk_init() {
	long long i, j;
	dict_fixed_t one_term;
	node_ptr_t node_ptr;

	//
	// (1) open the dict
	//
	dict_file = fopen(DICT_FIXED_FILENAME, "rb");
	if (!dict_file) { fprintf(stderr, "ERROR open file %s\n", DICT_FIXED_FILENAME); exit(2); }

	//
	// (2) find out the total number of terms
	//
	fseek(dict_file, 0, SEEK_END);
	term_count = ftell(dict_file) / sizeof(dict_fixed_t);
	dbg_printf("term_count: %lld\n", term_count);
	fseek(dict_file, 0, SEEK_SET);

	//
	// (3) calculate required number of nodes
	//
	node_max_size = hd_sector_size * num_of_sectors;
	node_length = node_max_size / sizeof(dict_fixed_t);
	dbg_printf("node_length: %lld\n", node_length);
	node_count = floorl(term_count/node_length);
	node_count = (term_count % node_length) ? (node_count + 1) : node_count;
	dbg_printf("node_count: %lld\n", node_count);

	//
	// (4) allocate memory for headers and node
	//
	headers_array = (dict_fixed_header_t *)malloc(node_count *sizeof(*headers_array));
	if (!headers_array) { perror("malloc for headers_array\n"); exit(2); }
	node_buffer = (dict_fixed_t *)malloc(node_length * sizeof(dict_fixed_t));
	if (!node_buffer) { perror("malloc for node_buffer\n"); exit(2); }

	//
	// (5) only read header terms
	//
	for ( i = 0, j = 0; i < term_count; i++) {
		fread(&one_term, sizeof(one_term), 1, dict_file);
		if ((i % node_length) == 0) {
			//printf("j: %lld\n", j);
			strcpy(headers_array[j].term, one_term.term);
			node_ptr = ftell(dict_file) - sizeof(one_term);
			headers_array[j].node_ptr = node_ptr;
			j++;
		}
	}

	stats.io_time = 0;
	stats.search_time = 0;
	stats.bytes_read = 0;

#if 0
	for (i = 0; i < node_count; i++) {
		printf("(%lld): %s\n", i, headers_array[i].term);
	}
#endif


#if (USE_O_DIRECT == 1)
	fclose(dict_file);
	free(node_buffer);
	int page_size = sysconf(_SC_PAGESIZE);

	if (posix_memalign(&aligned_buffer, page_size, node_max_size) != 0) {
		perror("posix_memalign for node_buffer\n");
		exit(2);
	}

	if ((dict_file_odirect = open(DICT_FIXED_FILENAME, O_RDONLY | O_DIRECT)) , 0) {
		fprintf(stderr, "open with O_DIRECT for %s\n", DICT_FIXED_FILENAME);
		exit(2);
	}
#endif


	if (run_command("flush_cache.sh") < 0) {
		perror("ERROR: calling flush_cache.sh");
		exit(2);
	}

}


int search_fixed_ondisk(char *the_term) {
	long long low, high, mid, terms;

	gettimeofday(&stats.start, NULL);
	//
	// (1) search the first level, the headers
	//
	low = 0;
	high = node_count -1;
	while (low <= high) {
		mid = (low + high) / 2;
		//printf("low: %lld, high: %lld, mid: %lld, term: %s\n", low, high, mid, headers_array[mid].term);
		if (strcmp(headers_array[mid].term, the_term) > 0) {
			high = mid - 1;
		}  else if (strcmp(headers_array[mid].term, the_term) < 0) {
			low = mid + 1;
		} else {
			// found in the headers, cannot just return now
			// need to load the secondary node, so that the postings pointer can be retrieved
			//return 1;
			break;
		}
	}

	//
	// (2) check if the term is in the previous block
	//
	if (strcmp(headers_array[mid].term, the_term) > 0) {
		mid = mid - 1;
	}
	gettimeofday(&stats.end, NULL);
	add_search_time();

	//
	// (3) search the second level
	//
	gettimeofday(&stats.start, NULL);
	// the last node might not be a full node
	if (mid == node_count - 1) {
		terms = (term_count - mid*node_length) > node_length ? node_length : (term_count - mid*node_length);
	} else {
		terms = node_length;
	}
#if (USE_O_DIRECT == 1)
	lseek(dict_file_odirect, headers_array[mid].node_ptr, SEEK_SET);
	read(dict_file_odirect, aligned_buffer, node_max_size);
	//int ret = read(dict_file_odirect, aligned_buffer, node_max_size);
	//printf("ret (read bytes): %d\n", ret);
	node_buffer = (dict_fixed_t *)aligned_buffer;
#else
	fseek(dict_file, headers_array[mid].node_ptr, SEEK_SET);
	fread(node_buffer, sizeof(*node_buffer), terms, dict_file);
#endif
	gettimeofday(&stats.end, NULL);
	add_io_time();

	add_bytes_read(sizeof(*node_buffer)*terms);

	gettimeofday(&stats.start, NULL);
	low = 0;
	high = terms -1;
	while (low <= high) {
		mid = (low + high) / 2;
		//printf("==> low: %lld, high: %lld, mid: %lld, term: %s\n", low, high, mid, node_buffer[mid].term);
		if (strcmp(node_buffer[mid].term, the_term) > 0) {
			high = mid - 1;
		}  else if (strcmp(node_buffer[mid].term, the_term) < 0) {
			low = mid + 1;
		} else {
			gettimeofday(&stats.end, NULL);
			add_search_time();
			return 1;
		}
	}

	gettimeofday(&stats.end, NULL);
	add_search_time();
	return 0;
}


void search_fixed_inmemory_init() {
	long long i = 0;
	dict_fixed_t one_term;

	//
	// (1) open the dict file
	//
	dict_file = fopen(DICT_FIXED_FILENAME, "rb");
	if (!dict_file) { fprintf(stderr, "ERROR open file %s\n", DICT_FIXED_FILENAME); exit(2); }

	//
	// (2) calculate the number of terms
	//
	dbg_printf("sizeof(dict_fixed_t): %lu\n", sizeof(dict_fixed_t));
	fseek(dict_file, 0, SEEK_END);
	term_count = ftell(dict_file) / sizeof(dict_fixed_t);
	dbg_printf("term_count: %lld\n", term_count);

	//
	// (3) memory allocation for all the terms
	//
	all_terms = (dict_fixed_t *)malloc(term_count * sizeof(*all_terms));
	if (!all_terms) { perror("malloc for all_terms\n"); exit(2); }

	//
	// (4)read all terms from the disk
	//
	fseek(dict_file, 0, SEEK_SET);
	while (fread(&one_term, sizeof(one_term), 1, dict_file)) {
		strcpy(all_terms[i].term, one_term.term);
		i++;
	}
	fclose(dict_file);


	stats.io_time = 0;
	stats.search_time = 0;
	stats.bytes_read = 0;

//#define TEST_FIXED_INIT
#ifdef TEST_FIXED_INIT
	for (long long i = 0; i < term_count; i++) {
		printf("%s\n", all_terms[i].term);
	}
#endif
}


int search_fixed_inmemory(char *the_term) {
	long long low, high, mid;

	gettimeofday(&stats.start, NULL);

	low = 0;
	high = term_count -1;
	while (low <= high) {
		mid = (low + high) / 2;
		if (strcmp(all_terms[mid].term, the_term) > 0) {
			high = mid - 1;
		} else if (strcmp(all_terms[mid].term, the_term) < 0) {
			low = mid + 1;
		} else {
			// found in the headers, just return
			gettimeofday(&stats.end, NULL);
			add_search_time();
			return 1;
		}
	}

	// not found
	gettimeofday(&stats.end, NULL);
	add_search_time();
	return 0;
}
