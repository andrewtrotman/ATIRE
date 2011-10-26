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
static long long term_count;
static uint32_t node_count, one_more_node_count;
static node_length_t node_length;
static unsigned long long headers_start, max_node_length;
static char *node_buffer, *node_buffer_ptr;
static dict_embedfixed_null_t *headers_array;
static term_t *term_list;
static term_t **term_list_ptr;
static char dummy_postings_ptr[POSTING_PTR_SIZE];
static vocab_loc_t *vocab_loc_ptr;

static term_t **find_end_of_block(term_t **start);

void build_embedfixed_null() {
	long long i, nodes_with_one_term;
    term_t **start, **end;
	dict_embedfixed_null_t *current_node;
	char null_value = '\0';

	//
	// (1) read term_count
	//
	printf("common_prefix_size: %ld\n", (long)COMMON_PREFIX_SIZE);
	printf("    node_max_size: %ld\n", hd_sector_size * num_of_sectors);
	vocab_file = fopen(VOCAB_FILENAME, "rb");
	dict_file = fopen(DICT_EMBEDFIXED_NULL_FILENAME, "wb");
	fread(&term_count, sizeof(term_count), 1, vocab_file);
	dbg_printf("term_count: %lld\n", term_count);

	printf("building embedfixed......\n");

    //
    // (2) hold all terms and assign term list pointers
    //
	term_list = (term_t *) malloc((term_count) * sizeof(*term_list));
	term_list_ptr = (term_t **) malloc((term_count + 1) * sizeof(**term_list_ptr));
	for (i = 0; i < term_count; i++) {
		fread(term_list[i].term, MAX_WORD_LENGTH+1, 1, vocab_file);
		term_list_ptr[i] = &term_list[i];
	}
	term_list_ptr[term_count + 1] = NULL;

    //
    // (3) find the number of blocks required, allocate required blocks, and create node buffer
    //
    node_count = 0;
    start = term_list_ptr;
    max_node_length = 0;
    while (*start != NULL) {
    	end = find_end_of_block(start);
    	node_count++;
    	if ((end - start) > max_node_length) {
    		max_node_length = (node_length_t)(end - start);
    	}
    	start = end;
    }
	headers_array = (dict_embedfixed_null_t *)malloc(sizeof(*headers_array)*node_count);
	dbg_printf("node_count: %d\n", node_count);
	dbg_printf("max_node_length: %llu\n", max_node_length);
	long long one_term_size = POSTING_PTR_SIZE + sizeof(vocab_loc_t) + MAX_WORD_LENGTH + 1;
	node_buffer = (char *)malloc(max_node_length * one_term_size + sizeof(node_length_t));

	//
	// (4) read all terms, build up the headers and nodes
	//
	nodes_with_one_term = 0;
	term_t **cur_term; long rest;
	start = term_list_ptr;
	current_node = headers_array;
	i = 0;
	while (*start != NULL) {
		// initialise node buffer
		node_buffer_ptr = node_buffer;

		// create the header for the current node
		// terms in the headers are null terminated
		current_node->prefix_length = strlen((*start)->term) >= COMMON_PREFIX_SIZE ? COMMON_PREFIX_SIZE_ONE_LESS : strlen((*start)->term);
		current_node->prefix = (char *)malloc(current_node->prefix_length + 1);
		strncpy((char *)current_node->prefix, (*start)->term, current_node->prefix_length);
		current_node->prefix[current_node->prefix_length] = '\0';
		current_node->node_ptr = ftell(dict_file);

		// find out how many terms there are in the node
		end = find_end_of_block(start);
		node_length = (node_length_t)(end - start);

		// when the node length is one, only postings pointers and suffix are required.
		// otherwise write the full structure in the node
		if (node_length == 1) {
			memcpy(node_buffer_ptr, dummy_postings_ptr, sizeof(dummy_postings_ptr));
			node_buffer_ptr += sizeof(dummy_postings_ptr);

			// the null terminator is also counted for term length
			// the COMMON_PREFIX_SIZE is one less because the header is null-terminated
			rest = (strlen((*start)->term)+1)-COMMON_PREFIX_SIZE_ONE_LESS;
			if (rest <= 0) {
				memcpy(node_buffer_ptr, &null_value, 1);
				node_buffer_ptr++;
			} else {
				memcpy(node_buffer_ptr, &(((*start)->term)[COMMON_PREFIX_SIZE_ONE_LESS]), rest);
				node_buffer_ptr += rest;
			}

			nodes_with_one_term++;

		} else {
			// write postings pointers for all the terms
			for (i = 0; i < node_length; i++) {
				memcpy(node_buffer_ptr, dummy_postings_ptr, sizeof(dummy_postings_ptr));
				node_buffer_ptr += sizeof(dummy_postings_ptr);
			}

			// reserve spaces for the vocab pointers
			vocab_loc_ptr = (vocab_loc_t *)node_buffer_ptr;
			node_buffer_ptr += node_length * sizeof(vocab_loc_t);

			// write terms suffixes and update vocab pointers
			for (cur_term = start; cur_term < end; cur_term++) {
				*vocab_loc_ptr = node_buffer_ptr - node_buffer;
				// the null terminator is also counted for term length
				rest = (strlen((*cur_term)->term)+1)-COMMON_PREFIX_SIZE_ONE_LESS;
				memcpy(node_buffer_ptr, &(((*cur_term)->term)[COMMON_PREFIX_SIZE_ONE_LESS]), rest);
				node_buffer_ptr += rest;
				vocab_loc_ptr++;
			}
		}

		// write node length to the end of the node
		memcpy(node_buffer_ptr, &node_length, sizeof(node_length));
		node_buffer_ptr += sizeof(node_length);

		// write the filled buffer to disk
		fwrite(node_buffer, node_buffer_ptr - node_buffer, 1, dict_file);

		// proceed to the next
		start = end;
		current_node++;
	} // end of while (*start != NULL)

	//
	// (5) write the updated node pointers to disk
	//
	headers_start = ftell(dict_file);
	// no need to store prefix_length, the length of prefixes can be restored at run-time
	for ( i = 0 ; i < node_count; i++) {
		// also write null terminator
		fwrite((unsigned char *)headers_array[i].prefix, headers_array[i].prefix_length+1, 1, dict_file);
		fwrite(&headers_array[i].node_ptr, sizeof(headers_array[i].node_ptr), 1, dict_file);
	}
	dbg_printf("headers_start: %lld, end_of_headers: %ld\n", headers_start, ftell(dict_file));

#if 0
	for (i = 0; i < node_count; i++) {
		printf("header[%lld]: %s\n", i, headers_array[i].prefix);
	}
	exit(2);
#endif

	//
	// (6) write the last three variable for the strucutre
	//
	fwrite(&node_count, sizeof(node_count), 1, dict_file);
	fwrite(&max_node_length, sizeof(max_node_length), 1, dict_file);
	fwrite(&headers_start, sizeof(headers_start), 1, dict_file);

	//
	// (7) finished
	//
	printf("     total_wasted: %d bytes\n", 0);
	fseek(dict_file, 0, SEEK_END);
	printf("  total file size: %ld\n", ftell(dict_file));
	printf("nodes with one term: %lld\n", nodes_with_one_term);
	fclose(dict_file);
	fclose(vocab_file);
    free(term_list_ptr);
	free(term_list);
	free(headers_array);
	free(node_buffer);
	printf("FINISHED\n\n");
}

static term_t **find_end_of_block(term_t **start) {
    term_t **current = start;

	if (strlen((*current)->term) < COMMON_PREFIX_SIZE_ONE_LESS) {
		current++;
	} else {
		while (*current != NULL) {
			if (strlen((*current)->term) < COMMON_PREFIX_SIZE_ONE_LESS)
				break;
			if (strncmp((*start)->term, (*current)->term, COMMON_PREFIX_SIZE_ONE_LESS) != 0)
				break;
			current++;
		}
	}
    return current;
}

static long long bytes_to_read;
static char *suffix_ptr;
static long long end_of_headers;
static char *buffer, *buffer_ptr;

void serialise_embedfixed_null() {
	FILE *serialised_file = NULL;
	long long i, j, n;


	printf("\nserialising embedfixed......");

	//
	// (1) open files
	//
	dict_file = fopen(DICT_EMBEDFIXED_NULL_FILENAME, "rb");
	if (!dict_file) {
		fprintf(stderr, "ERROR open file %s\n", DICT_EMBEDFIXED_NULL_FILENAME);
		exit(2);
	}
	serialised_file = fopen(SERIALISE_EMBEDFIXED_NULL_FILENAME, "wb");
	if (!serialised_file) {
		fprintf(stderr, "ERROR open file %s\n", SERIALISE_EMBEDFIXED_NULL_FILENAME);
		exit(2);
	}

	//
	// (2) read node_count, max_node_length and where the headers start
	//
	fseek(dict_file, (long long)(sizeof(node_count)+sizeof(max_node_length)+sizeof(headers_start)) * (-1), SEEK_END);
	end_of_headers = ftell(dict_file);
	fread(&node_count, sizeof(node_count), 1, dict_file);
	one_more_node_count = node_count + 1;
	dbg_printf("node_count: %d, one_more_node_count: %d\n", node_count, one_more_node_count);
	fread(&max_node_length, sizeof(max_node_length), 1, dict_file);
	dbg_printf("max_node_length: %lld\n", max_node_length);
	fread(&headers_start, sizeof(headers_start), 1, dict_file);
	dbg_printf("headers_start: %lld, end_of_headers: %lld\n", headers_start, end_of_headers);

	//
	// (3) setup the headers and the last dummy header to point to the end of the headers
	//
	fseek(dict_file, headers_start, SEEK_SET);
	buffer = (char *)malloc(end_of_headers - headers_start);
	fread(buffer, (end_of_headers - headers_start), 1, dict_file);
	headers_array = (dict_embedfixed_null_t *)malloc(one_more_node_count * sizeof(*headers_array));
	buffer_ptr = buffer;
	for (i = 0; i < node_count; i++) {
		n = strlen((char *)buffer_ptr);
		headers_array[i].prefix = (char *)malloc(n+1);
		strcpy(headers_array[i].prefix, buffer_ptr);
		headers_array[i].prefix_length = n;
		buffer_ptr += (n+1);
		headers_array[i].node_ptr = *(node_ptr_t *)buffer_ptr;
		buffer_ptr += sizeof(node_ptr_t);
	}
	free(buffer);
	fseek(dict_file, headers_start, SEEK_SET);
	headers_array[one_more_node_count-1].node_ptr = ftell(dict_file);

	//
	// (4) setup the node buffer
	//
	long long one_term_size = POSTING_PTR_SIZE + sizeof(vocab_loc_t) + MAX_WORD_LENGTH + 1;
	node_buffer = (char *)malloc(max_node_length * one_term_size + sizeof(node_length_t));

#if 0
	for (i = 0; i < node_count; i++) {
		printf("header[%lld]: %s\n", i, headers_array[i].prefix);
	}
	exit(2);
#endif

	//
	// (5) read one node at a time, and serialise the terms in the node
	//
	for (i = 0; i < node_count; i++) {
		//
		// read the node from file
		//
		fseek(dict_file, headers_array[i].node_ptr, SEEK_SET);
		bytes_to_read = headers_array[i+1].node_ptr - headers_array[i].node_ptr;
		fread(node_buffer, bytes_to_read, 1, dict_file);

		//
		// find out node length
		//
		node_buffer_ptr = node_buffer + bytes_to_read - sizeof(node_length);
		node_length = *(node_length_t *)node_buffer_ptr;

		//
		// print all terms
		//
		if (node_length == 1) {
			fwrite(headers_array[i].prefix, strlen(headers_array[i].prefix), 1, serialised_file);
			suffix_ptr = node_buffer+POSTING_PTR_SIZE;
			if (strlen(suffix_ptr) > 0) {
				fwrite(suffix_ptr, strlen(suffix_ptr), 1, serialised_file);
			}
			fwrite("\n", 1, 1, serialised_file);
		} else {
			vocab_loc_ptr = (vocab_loc_t *)(node_buffer + node_length * POSTING_PTR_SIZE);
			for (j = 0; j < node_length; j++) {
				// write the prefix
				fwrite(headers_array[i].prefix, strlen(headers_array[i].prefix), 1, serialised_file);
				suffix_ptr = &node_buffer[*vocab_loc_ptr];
				fwrite(suffix_ptr, strlen(suffix_ptr), 1, serialised_file);
				fwrite("\n", 1, 1, serialised_file);
				vocab_loc_ptr++;
			}
		}
	} // end of for (i = 0; i < node_length)

	//
	// (6) finished
	//
	fclose(serialised_file);
	fclose(dict_file);
	for (i = 0; i < node_count; i++) {
		free(headers_array[i].prefix);
	}
	free(headers_array);
	free(node_buffer);
	printf("\n");
}


static dict_embedfixed_t *h_array;
#if (USE_O_DIRECT == 1)
	static int dict_file_odirect;
	static void *aligned_buffer;
	static long long new_start_loc;
	static long long offset_start, offset_end;
#endif

void search_embedfixed_null_ondisk_init() {
	long long i, n;

	//
	// (1) open the dict file
	//
	dict_file = fopen(DICT_EMBEDFIXED_NULL_FILENAME, "rb");
	if (!dict_file) {
		fprintf(stderr, "ERROR open file %s\n", DICT_EMBEDFIXED_NULL_FILENAME);
		exit(2);
	}

	//
	// (2) read node_count, max_node_length and where the headers start
	//
	fseek(dict_file, (long long)(sizeof(node_count)+sizeof(max_node_length)+sizeof(headers_start)) * (-1), SEEK_END);
	end_of_headers = ftell(dict_file);
	fread(&node_count, sizeof(node_count), 1, dict_file);
	one_more_node_count = node_count + 1;
	dbg_printf("node_count: %d, one_more_node_count: %d\n", node_count, one_more_node_count);
	fread(&max_node_length, sizeof(max_node_length), 1, dict_file);
	dbg_printf("max_node_length: %lld\n", max_node_length);
	fread(&headers_start, sizeof(headers_start), 1, dict_file);
	dbg_printf("headers_start: %lld, end_of_headers: %lld\n", headers_start, end_of_headers);

	//
	// (3) setup the headers and the last dummy header to point to the end of the headers
	//
	fseek(dict_file, headers_start, SEEK_SET);
	buffer = (char *)malloc(end_of_headers - headers_start);
	fread(buffer, (end_of_headers - headers_start), 1, dict_file);
	h_array = (dict_embedfixed_t *)malloc(one_more_node_count * sizeof(*h_array));
	buffer_ptr = buffer;
	for (i = 0; i < node_count; i++) {
		n = strlen((char *)buffer_ptr);
		strcpy(h_array[i].prefix, buffer_ptr);
		//h_array[i].prefix_length = n;
		buffer_ptr += (n+1);
		h_array[i].node_ptr = *(node_ptr_t *)buffer_ptr;
		buffer_ptr += sizeof(node_ptr_t);
	}
	free(buffer);
	fseek(dict_file, headers_start, SEEK_SET);
	h_array[one_more_node_count-1].node_ptr = ftell(dict_file);

	//
	// (4) setup the node buffer
	//
	long long one_term_size = POSTING_PTR_SIZE + sizeof(vocab_loc_t) + MAX_WORD_LENGTH + 1;
	node_buffer = (char *)malloc(max_node_length * one_term_size + sizeof(node_length_t));

	stats.io_time = 0;
	stats.search_time = 0;
	stats.bytes_read = 0;

#if 0
	for (i = 0; i < node_count; i++) {
		printf("header[%lld]: %s\n", i, headers_array[i].prefix);
	}
	exit(2);
#endif

#if (USE_O_DIRECT == 1)
	fclose(dict_file);
	free(node_buffer);
	int page_size = sysconf(_SC_PAGESIZE);

	if (posix_memalign(&aligned_buffer, page_size, (max_node_length * one_term_size + sizeof(node_length_t) + hd_sector_size*2)) != 0) {
		perror("posix_memalign for node_buffer\n");
		exit(2);
	}

	if ((dict_file_odirect = open(DICT_EMBEDFIXED_NULL_FILENAME, O_RDONLY | O_DIRECT)) , 0) {
		fprintf(stderr, "open with O_DIRECT for %s\n", DICT_EMBEDFIXED_NULL_FILENAME);
		exit(2);
	}
#endif
}

int search_embedfixed_null_ondisk(char *the_term) {
	long long low, high, mid, rest, n;

	gettimeofday(&stats.start, NULL);
	//
	// (1) search the first level, the headers
	//
	low = 0;
	high = node_count - 1;
	while (low <= high) {
		mid = (low + high) / 2;
		if (strcmp(h_array[mid].prefix, the_term) > 0) {
			high = mid - 1;
		} else if (strcmp(h_array[mid].prefix, the_term) < 0) {
			low = mid + 1;
		} else {
			//found in the headers, need to search in the node
			//return 1;
			break;
		}
	}

	//
	// (2) check if the term is in the previous block
	//
	if (strcmp(h_array[mid].prefix, the_term) > 0) {
		mid = mid - 1;
	}
	//printf("i: %lld, term: %s\n", mid, h_array[mid].prefix);
	gettimeofday(&stats.end, NULL);
	add_search_time();

	//
	// (3) search the second level
	//
#if (USE_O_DIRECT == 1)
	// locate the beginning of the sector for the node
	offset_start = h_array[mid].node_ptr % hd_sector_size;
	if (offset_start != 0) {
		new_start_loc = h_array[mid].node_ptr - offset_start;
	} else {
		new_start_loc = h_array[mid].node_ptr;
	}

	// locate the end of the sector for the node
	offset_end = h_array[mid+1].node_ptr % hd_sector_size;
	if (offset_end != 0) {
		bytes_to_read = (h_array[mid+1].node_ptr + (hd_sector_size - offset_end)) - new_start_loc;
	} else {
		bytes_to_read = h_array[mid+1].node_ptr - new_start_loc;
	}

	// read the requried number of sectors for the node
	gettimeofday(&stats.start, NULL);
	lseek(dict_file_odirect, new_start_loc, SEEK_SET);
	read(dict_file_odirect, aligned_buffer, bytes_to_read);
	//int ret = read(dict_file_odirect, aligned_buffer, bytes_to_read);
	//printf("ret (read bytes): %d\n", ret);
	//printf("mid: %ld, mid+1: %ld, new_start_loc: %lld, bytes_to_read: %lld\n",
	//		h_array[mid].node_ptr, h_array[mid+1].node_ptr,
	//		new_start_loc, bytes_to_read);

	// locate where the node starts in the sector
	node_buffer = (char *)aligned_buffer + offset_start;

	// the read size for the node
	bytes_to_read = h_array[mid+1].node_ptr - h_array[mid].node_ptr;
	gettimeofday(&stats.end, NULL);
#else
	gettimeofday(&stats.start, NULL);
	fseek(dict_file, h_array[mid].node_ptr, SEEK_SET);
	bytes_to_read = h_array[mid+1].node_ptr - h_array[mid].node_ptr;
	fread(node_buffer, bytes_to_read, 1, dict_file);
	gettimeofday(&stats.end, NULL);
#endif
	add_io_time();

	add_bytes_read(bytes_to_read);

	gettimeofday(&stats.start, NULL);
	// find out node length
	node_buffer_ptr = node_buffer + bytes_to_read - sizeof(node_length);
	node_length = *(node_length_t *)node_buffer_ptr;

	// search the suffixes in the node
	if (node_length == 1) {
		// the null terminator is also counted for term length
		n = strlen(the_term);
		rest = n + 1 - COMMON_PREFIX_SIZE_ONE_LESS;
		if (rest <= 0) {
			if (strcmp(h_array[mid].prefix, the_term) == 0) {
				gettimeofday(&stats.end, NULL);
				add_search_time();
				return 1;
			}
		} else {
			suffix_ptr = node_buffer+POSTING_PTR_SIZE;
			if ((strncmp(h_array[mid].prefix, the_term, COMMON_PREFIX_SIZE_ONE_LESS) == 0) && (strcmp(suffix_ptr, &the_term[COMMON_PREFIX_SIZE_ONE_LESS]) == 0)) {
				gettimeofday(&stats.end, NULL);
				add_search_time();
				return 1;
			}
		}
		gettimeofday(&stats.end, NULL);
		add_search_time();
		return 0;

	} else {
		if (strncmp(h_array[mid].prefix, the_term, COMMON_PREFIX_SIZE_ONE_LESS) != 0) {
			gettimeofday(&stats.end, NULL);
			add_search_time();
			return 0;
		}
		vocab_loc_ptr = (vocab_loc_t *)(node_buffer + node_length * POSTING_PTR_SIZE);
		low = 0;
		high = node_length - 1;
		while (low <= high) {
			mid = (low + high) / 2;
			if (strcmp(&node_buffer[vocab_loc_ptr[mid]], &the_term[COMMON_PREFIX_SIZE_ONE_LESS]) > 0) {
				high = mid - 1;
			} else if (strcmp(&node_buffer[vocab_loc_ptr[mid]], &the_term[COMMON_PREFIX_SIZE_ONE_LESS]) < 0) {
				low = mid + 1;
			} else {
				// found
				gettimeofday(&stats.end, NULL);
				add_search_time();
				return 1;
			}
		}

		gettimeofday(&stats.end, NULL);
		add_search_time();
		return 0;
	}

	return 0;
}

static char *all_nodes = NULL;

void search_embedfixed_null_inmemory_init() {
	long long i, n;

	//
	// (1) open the dict file
	//
	dict_file = fopen(DICT_EMBEDFIXED_NULL_FILENAME, "rb");
	if (!dict_file) {
		fprintf(stderr, "ERROR open file %s\n", DICT_EMBEDFIXED_NULL_FILENAME);
		exit(2);
	}

	//
	// (2) read node_count, max_node_length and where the headers start
	//
	fseek(dict_file, (long long)(sizeof(node_count)+sizeof(max_node_length)+sizeof(headers_start)) * (-1), SEEK_END);
	end_of_headers = ftell(dict_file);
	fread(&node_count, sizeof(node_count), 1, dict_file);
	one_more_node_count = node_count + 1;
	dbg_printf("node_count: %d, one_more_node_count: %d\n", node_count, one_more_node_count);
	fread(&max_node_length, sizeof(max_node_length), 1, dict_file);
	dbg_printf("max_node_length: %lld\n", max_node_length);
	fread(&headers_start, sizeof(headers_start), 1, dict_file);
	dbg_printf("headers_start: %lld, end_of_headers: %lld\n", headers_start, end_of_headers);

	//
	// (3) setup the headers and the last dummy header to point to the end of the headers
	//
	fseek(dict_file, headers_start, SEEK_SET);
	buffer = (char *)malloc(end_of_headers - headers_start);
	fread(buffer, (end_of_headers - headers_start), 1, dict_file);
	h_array = (dict_embedfixed_t *)malloc(one_more_node_count * sizeof(*h_array));
	buffer_ptr = buffer;
	for (i = 0; i < node_count; i++) {
		n = strlen((char *)buffer_ptr);
		strcpy(h_array[i].prefix, buffer_ptr);
		//h_array[i].prefix_length = n;
		buffer_ptr += (n+1);
		h_array[i].node_ptr = *(node_ptr_t *)buffer_ptr;
		buffer_ptr += sizeof(node_ptr_t);
	}
	free(buffer);
	fseek(dict_file, headers_start, SEEK_SET);
	h_array[one_more_node_count-1].node_ptr = ftell(dict_file);

#if 0
	for (i = 0; i < node_count; i++) {
		printf("header[%lld]: %s\n", i, headers_array[i].prefix);
	}
	exit(2);
#endif

	//
	// (4) read all nodes in memory
	//
	//long long one_term_size = POSTING_PTR_SIZE + sizeof(vocab_loc_t) + MAX_WORD_LENGTH + 1;
	//node_buffer = (char *)malloc(max_node_length * one_term_size + sizeof(node_length_t));
	all_nodes = (char *)malloc(end_of_headers);
	if (!all_nodes) {
		perror("ERROR: malloc for all_nodes\n");
		exit(2);
	}
	// reset the fd to the beginning of the file
	fseek(dict_file, 0, SEEK_SET);
	fread(all_nodes, end_of_headers, 1, dict_file);

	stats.io_time = 0;
	stats.search_time = 0;
	stats.bytes_read = 0;
}

int search_embedfixed_null_inmemory(char *the_term) {
	long long low, high, mid, rest, n;

	gettimeofday(&stats.start, NULL);
	//
	// (1) search the first level, the headers
	//
	low = 0;
	high = node_count - 1;
	while (low <= high) {
		mid = (low + high) / 2;
		if (strcmp(h_array[mid].prefix, the_term) > 0) {
			high = mid - 1;
		} else if (strcmp(h_array[mid].prefix, the_term) < 0) {
			low = mid + 1;
		} else {
			//found in the headers, need to search in the node
			//return 1;
			break;
		}
	}

	//
	// (2) check if the term is in the previous block
	//
	if (strcmp(h_array[mid].prefix, the_term) > 0) {
		mid = mid - 1;
	}
	//printf("i: %lld, term: %s\n", mid, headers_array[mid].prefix);
	gettimeofday(&stats.end, NULL);
	add_search_time();

	//
	// (3) search the second level
	//
	gettimeofday(&stats.start, NULL);

	// locate the node in memory
	node_buffer = all_nodes + h_array[mid].node_ptr;
	bytes_to_read = h_array[mid+1].node_ptr - h_array[mid].node_ptr;

	// find out node length
	node_buffer_ptr = node_buffer + bytes_to_read - sizeof(node_length);
	node_length = *(node_length_t *)node_buffer_ptr;

	// search the suffixes in the node
	if (node_length == 1) {
		// the null terminator is also counted for term length
		n = strlen(the_term);
		rest = n + 1 - COMMON_PREFIX_SIZE_ONE_LESS;
		if (rest <= 0) {
			if (strcmp(h_array[mid].prefix, the_term) == 0) {
				gettimeofday(&stats.end, NULL);
				add_search_time();
				return 1;
			}
		} else {
			suffix_ptr = node_buffer+POSTING_PTR_SIZE;
			if ((strncmp(h_array[mid].prefix, the_term, COMMON_PREFIX_SIZE_ONE_LESS) == 0) && (strcmp(suffix_ptr, &the_term[COMMON_PREFIX_SIZE_ONE_LESS]) == 0)) {
				gettimeofday(&stats.end, NULL);
				add_search_time();
				return 1;
			}
		}
		gettimeofday(&stats.end, NULL);
		add_search_time();
		return 0;

	} else {
		if (strncmp(h_array[mid].prefix, the_term, COMMON_PREFIX_SIZE_ONE_LESS) != 0) {
			gettimeofday(&stats.end, NULL);
			add_search_time();
			return 0;
		}
		vocab_loc_ptr = (vocab_loc_t *)(node_buffer + node_length * POSTING_PTR_SIZE);
		low = 0;
		high = node_length - 1;
		while (low <= high) {
			mid = (low + high) / 2;
			if (strcmp(&node_buffer[vocab_loc_ptr[mid]], &the_term[COMMON_PREFIX_SIZE_ONE_LESS]) > 0) {
				high = mid - 1;
			} else if (strcmp(&node_buffer[vocab_loc_ptr[mid]], &the_term[COMMON_PREFIX_SIZE_ONE_LESS]) < 0) {
				low = mid + 1;
			} else {
				// found
				gettimeofday(&stats.end, NULL);
				add_search_time();
				return 1;
			}
		}

		gettimeofday(&stats.end, NULL);
		add_search_time();
		return 0;
	}
}

