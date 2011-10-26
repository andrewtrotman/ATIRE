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
#include <math.h>
#include "common.h"
#include "linked_list.h"
#if (USE_O_DIRECT == 1)
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
#endif

static FILE *dict_file;
static FILE *vocab_file;
static long long term_count;
static Linked_list<dict_blocked_t *> *headers_list;
static Linked_list<char *> *nodes_list;
static uint32_t node_count = 0;
static node_length_t node_length = 0;
static vocab_loc_t vocab_loc;

void build_blocked() {
	char one_byte = '\0';
	char term[MAX_WORD_LENGTH+1];
	long long i, j;
	dict_blocked_t *one_header;
	char *node_buffer = NULL, *term_buffer = NULL, *node_ptr = NULL, *term_ptr = NULL;
	char *block_buffer = NULL, *block_ptr = NULL;
	long long node_max_size;
	long long needed_size;

	if (block_size <= 0) {
		fprintf(stderr, "ERROR: block size cannot be zero or less\n");
		exit(2);
	}

	node_max_size = hd_sector_size * num_of_sectors;
	printf("    node_max_size: %lld\n", node_max_size);
	term_buffer = (char *)malloc(node_max_size);
	headers_list = new Linked_list<dict_blocked_t *>();
	nodes_list = new Linked_list<char *> ();
	// a term has a maximum size of MAX_WORD_LENGTH + 1
	// the other extra one byte is for count the size of term
	block_buffer = (char *)malloc(block_size * (MAX_WORD_LENGTH+1+1+1));

	//
	// make sure a single block cannot exceed the node size
	//
	if ((sizeof(vocab_loc_t) + (block_size * POSTING_PTR_SIZE) + (block_size * (MAX_WORD_LENGTH+1+1+1))) > node_max_size) {
		perror("ERROR: the block size cannot exceed the node size\n");
		exit(4);
	}

	printf("building blocked......\n");

	//
	// (1) read term_count
	//
	vocab_file = fopen(VOCAB_FILENAME, "rb");
	dict_file = fopen(DICT_BLOCKED_FILENAME, "wb");
	fread(&term_count, sizeof(term_count), 1, vocab_file);
	dbg_printf("term_count: %lld\n", term_count);

	//
	// (2) read all terms, build up the headers and nodes
	//
	i = 0;
	char filled_previously = TRUE;
	char new_node = TRUE;
	char node_is_full = FALSE, the_end = FALSE;
	node_count = 0;
	long long total_wasted = 0;
	long long terms_start_at = 0;
	long long real_block_size = 0;
	while (i <= term_count) {
		//
		// (2-1) make sure the previous block is filled in properly before read the next block from file
		//
		if (filled_previously) {
			block_ptr = block_buffer;
			// please note that the last block might not be fully filled
			// so, need to check the counter variable j
			for (j = 0; (j < block_size) && (i < term_count); j++, i++) {
				fread(term, MAX_WORD_LENGTH + 1, 1, vocab_file);
				*block_ptr = (uint8_t) strlen(term);
				block_ptr++;
				strncpy(block_ptr, term, strlen(term));
				block_ptr += strlen(term);
			}
			real_block_size = j;

#if 0
			int z, size;
			char *temp_ptr = block_buffer;
			for (z = 0; z < block_size; z++) {
				size = (int)*temp_ptr;
				printf("block: (%d) ", size);
				temp_ptr++;
				strncpy(term, temp_ptr, size);
				term[size] = '\0';
				printf("%s\n", term);
				temp_ptr += size;
			}
#endif

			if (i == term_count) {
				the_end = TRUE;
			}
		}

		//
		// (2-2) create a new header and the new associated node
		//
		if (new_node) {
			//one_header = (dict_string_t *) malloc(sizeof(*one_header));
			one_header = new dict_blocked_t;
			strncpy(one_header->term, &block_buffer[1], block_buffer[0]);
			one_header->term[(int)block_buffer[0]] = '\0';
			headers_list->append(one_header);
			node_count++;

			//node_buffer =  (char *)malloc(sizeof(*node_buffer)*max_node_size*512);
			node_buffer = new char[node_max_size];
			nodes_list ->append(node_buffer);
			node_ptr = node_buffer + sizeof(node_length);
			term_ptr = term_buffer;
			node_length = 0;

			new_node = FALSE;
			node_is_full = FALSE;
		}

		//
		// (2-3) fill as many blocks as possible in the node
		//
		// need to check the real_block_size instead of block_size because the last block might not be fully filled
		needed_size = sizeof(vocab_loc_t) + real_block_size * POSTING_PTR_SIZE + (block_ptr - block_buffer);
		// check if there is enough space to fill in the current block in this node
		if (((node_ptr - node_buffer) + (term_ptr - term_buffer) + needed_size) < node_max_size) {
			// fill the node buffer
			vocab_loc = term_ptr - term_buffer;
			memcpy(node_ptr, &vocab_loc, sizeof(vocab_loc_t));
			node_ptr += sizeof(vocab_loc_t) + real_block_size * POSTING_PTR_SIZE;
			// fill the term buffer
			memcpy(term_ptr, block_buffer, block_ptr - block_buffer);
			term_ptr += (block_ptr - block_buffer);

			node_length++;
			filled_previously = TRUE;
		} else {
			node_is_full = TRUE;
			filled_previously = FALSE;
			// the current block is full, need to create a new one
			new_node = TRUE;
		}

		//
		// (2-4) the current node is full or no more term to be processed
		//
		// if node_is_full and the_end happen at the same time, then the last block will not be handled.
		// need to take one more iteration to create a new node and full the buffer and then exit.
		// This is why the while loop is set to i <= term_count.
		if (node_is_full || the_end) {
			// store node length at the beginning of the node
			memcpy(node_buffer, &node_length, sizeof(node_length));
			// find out how much space wasted
			total_wasted = node_max_size - (node_ptr - node_buffer) - (term_ptr - term_buffer);
			// find out where the terms start in the node
			terms_start_at = node_ptr - node_buffer;
			// copy the terms into the node
			memcpy(node_ptr, term_buffer, (term_ptr - term_buffer));

			// convert vocab_loc to point to the offsets of the terms in the node
			node_ptr = node_buffer + sizeof(node_length_t);
			for (j = 0; j < node_length; j++) {
				*(vocab_loc_t *)node_ptr += terms_start_at;
				node_ptr += sizeof(vocab_loc_t) + block_size * POSTING_PTR_SIZE;
			}
		}

		if ((the_end) && (filled_previously)) {
			break;
		}

#if 0
		if (node_is_full || the_end) {
			char *node_ptr = node_buffer, *term_ptr;
			node_length_t node_length_02;
			long long i, j;
			uint8_t size;
			vocab_loc_t vocab_loc;
			node_length_02 = *(node_length_t *) node_ptr;
			node_ptr += sizeof(node_length_t);
			printf("node_legnth: %d, node_legnth_02: %d\n", node_length, node_length_02);
			for (i = 0; i < node_length_02; i++) {
				vocab_loc = *(vocab_loc_t *) node_ptr;
				term_ptr = node_buffer + vocab_loc;

				for (j = 0; j < real_block_size; j++) {
					size = (uint8_t) *term_ptr;
					printf("block: (%d) ", size);
					term_ptr++;
					strncpy(term, term_ptr, size);
					term[size] = '\0';
					printf("%s\n", term);
					term_ptr += size;
				}
				node_ptr += sizeof(vocab_loc_t) + real_block_size * POSTING_PTR_SIZE;
			}

		}
#endif
	} // end of while (i < term_count)

	//
	// (3) write node_count and headers to disk
	//
	dbg_printf("node_count: %u\n", node_count);
	dict_blocked_t *h = NULL;
	fwrite(&node_count, sizeof(node_count), 1, dict_file);
	for (h = headers_list->first(); h != NULL; h = headers_list->next()) {
		fwrite(h->term, strlen(h->term)+1, 1, dict_file);
		fwrite(&(h->node_ptr), sizeof(h->node_ptr), 1, dict_file);
	}

	//
	// (4) fill in the gap to make sure nodes starting at the beginning of a sector
	//
	//long long padding = hd_sector_size - ftell(dict_file) % hd_sector_size;
	long long padding = node_max_size - ftell(dict_file) % node_max_size;
	total_wasted += padding;
	for (i = 0; i < padding; i++) {
		fwrite(&one_byte, 1, 1, dict_file);
	}

	//
	// (5) write nodes to disk and update node pointer in headers
	//
	char *n = NULL;
	for (n = nodes_list->first(), h = headers_list->first(); n != NULL; n = nodes_list->next(), h = headers_list->next()) {
		h->node_ptr = ftell(dict_file);
		fwrite(n, node_max_size, 1, dict_file);
	}

	//
	// (6) re-write node_count and headers to disk
	//
	fseek(dict_file, 0, SEEK_SET);
	fwrite(&node_count, sizeof(node_count), 1, dict_file);
	for (h = headers_list->first(); h != NULL; h = headers_list->next()) {
		fwrite(h->term, strlen(h->term) + 1, 1, dict_file);
		fwrite(&(h->node_ptr), sizeof(h->node_ptr), 1, dict_file);
	}

	//
	// (7) finished
	//
	printf("     total_wasted: %lld bytes\n", total_wasted);
	fseek(dict_file, 0, SEEK_END);
	printf("  total file size: %ld\n", ftell(dict_file));
	fclose(dict_file);
	free(term_buffer);
	free(block_buffer);
	delete headers_list;
	delete nodes_list;
	printf("FINISHED\n\n");
}

static dict_blocked_t *headers_array;

void serialise_blocked() {
	FILE *serialised_file = NULL;
	char *buffer;
	long long i, j, w, buffer_max_size;
	char term[MAX_WORD_LENGTH + 1];

	printf("\nserialising string......");

	//
	// (1) open files
	//
	dict_file = fopen(DICT_BLOCKED_FILENAME, "rb");
	if (!dict_file) {
		fprintf(stderr, "ERROR open file %s\n", DICT_BLOCKED_FILENAME);
		exit(2);
	}
	serialised_file = fopen(SERIALISE_BLOCKED_FILENAME, "wb");
	if (!serialised_file) {
		fprintf(stderr, "ERROR open file %s\n", SERIALISE_BLOCKED_FILENAME);
		exit(2);
	}

	//
	// (2) read header terms from file
	//
	fread(&node_count, sizeof(node_count), 1, dict_file);
	dbg_printf("node_count: %d\n", node_count);
	// header term has a maximum length of MAX_WORD_LENGTH + 1
	buffer_max_size = node_count * (MAX_WORD_LENGTH + 1 + sizeof(node_ptr_t));
	buffer = (char *) malloc(buffer_max_size);
	//buffer_max_size is the upper bound for the headers
	fread(buffer, buffer_max_size, 1, dict_file);

	//
	// (3) convert into proper header structures
	//
	headers_array = (dict_blocked_t *) malloc(node_count * sizeof(dict_blocked_t));
	char *buffer_ptr = buffer;
	for (i = 0; i < node_count; i++) {
		strncpy(headers_array[i].term, buffer_ptr, strlen(buffer_ptr) + 1);
		buffer_ptr += strlen(buffer_ptr) + 1;
		memcpy(&headers_array[i].node_ptr, buffer_ptr, sizeof(node_ptr_t));
		buffer_ptr += sizeof(node_ptr_t);
	}
	free(buffer);

#if 0
	for (i = 0; i < node_count; i++) {
		printf("header[%lld]: %s\n", i, headers_array[i].term);
	}
	exit(2);
#endif

	//
	// (4) read one node at a time, and serialize the terms in the node
	//
	buffer_max_size = hd_sector_size * num_of_sectors;
	buffer = (char *)malloc(buffer_max_size);
	char *term_ptr;
	int real_block_size;
	uint8_t n;
	for (i = 0; i < node_count; i++) {
		fseek(dict_file, headers_array[i].node_ptr, SEEK_SET);
		fread(buffer, buffer_max_size, 1, dict_file);
		memcpy(&node_length, buffer, sizeof(node_length_t));

		// locate where vocab pointer starts
		buffer_ptr = buffer + sizeof(node_length_t);

		for (j = 0; j < node_length; j++) {

			// locate where terms starts
			term_ptr = buffer + *((vocab_loc_t *) buffer_ptr);

			if ((i == (node_count - 1)) && (j == (node_length - 1))) {
				// *(vocab_loc_t *)(buffer + sizeof(node_loc_t)) is the first vocab pointer in the node.
				// the first vocab points to the beginning of the terms.
				// then find out how many postings pointers there are, the number of postings
				// points is the same as the block size.
				real_block_size = ((buffer + *(vocab_loc_t *) (buffer + sizeof(node_length_t))) - (buffer_ptr + sizeof(vocab_loc_t)));
				real_block_size = real_block_size / POSTING_PTR_SIZE;
			} else {
				real_block_size = block_size;
			}

			for (w = 0; w < real_block_size; w++) {
				n = *term_ptr;
				term_ptr++;
				strncpy(term, term_ptr, n);
				term[n] = '\0';
				fwrite(term, n, 1, serialised_file);
				fwrite("\n", 1, 1, serialised_file);
				term_ptr += n;
			}

			buffer_ptr += sizeof(vocab_loc_t) + real_block_size * POSTING_PTR_SIZE;
		}
	}
	free(buffer);

	//
	// (5) finished
	//
	fclose(dict_file);
	fclose(serialised_file);
	printf("\n");

}

static char *node_buffer, *node_ptr, *term_ptr;
static long long node_max_size;
static node_length_t *node_length_ptr;
static long long real_block_size;
#if (USE_O_DIRECT == 1)
	static int dict_file_odirect;
	static void *aligned_buffer;
#endif

void search_blocked_ondisk_init() {
	char *buffer;
	long long i, buffer_max_size;

	//
	// (1) open the dict file
	//
	dict_file = fopen(DICT_BLOCKED_FILENAME, "rb");
	if (!dict_file) {
		fprintf(stderr, "ERROR open file %s\n", DICT_BLOCKED_FILENAME);
		exit(2);
	}

	//
	// (2) read header terms from file
	//
	fread(&node_count, sizeof(node_count), 1, dict_file);
	dbg_printf("node_count: %d\n", node_count);
	// header term has a maximum length of MAX_WORD_LENGTH + 1
	buffer_max_size = node_count * (MAX_WORD_LENGTH + 1 + sizeof(node_ptr_t));
	buffer = (char *) malloc(buffer_max_size);
	//buffer_max_size is the upper bound for the headers
	fread(buffer, buffer_max_size, 1, dict_file);

	//
	// (3) convert into proper header structures
	//
	headers_array = (dict_blocked_t *) malloc(node_count * sizeof(dict_blocked_t));
	char *buffer_ptr = buffer;
	for (i = 0; i < node_count; i++) {
		strncpy(headers_array[i].term, buffer_ptr, strlen(buffer_ptr) + 1);
		buffer_ptr += strlen(buffer_ptr) + 1;
		memcpy(&headers_array[i].node_ptr, buffer_ptr, sizeof(node_ptr_t));
		buffer_ptr += sizeof(node_ptr_t);
	}
	free(buffer);

#if 0
	for (i = 0; i < node_count; i++) {
		printf("header[%lld]: %s\n", i, headers_array[i].term);
	}
	exit(2);
#endif

	//
	// (4) pre-allocate a node for use
	//
	node_max_size = hd_sector_size * num_of_sectors;
	node_buffer = (char *)malloc(node_max_size);

	stats.io_time = 0;
	stats.search_time = 0;
	stats.bytes_read = 0;

#if (USE_O_DIRECT == 1)
	fclose(dict_file);
	free(node_buffer);
	int page_size = sysconf(_SC_PAGESIZE);

	if (posix_memalign(&aligned_buffer, page_size, node_max_size) != 0) {
		perror("posix_memalign for node_buffer\n");
		exit(2);
	}

	if ((dict_file_odirect = open(DICT_BLOCKED_FILENAME, O_RDONLY | O_DIRECT)) , 0) {
		fprintf(stderr, "open with O_DIRECT for %s\n", DICT_BLOCKED_FILENAME);
		exit(2);
	}
#endif
}

int search_blocked_ondisk(char *the_term) {
	long long low, high, mid_header, mid, w, n;
	long long the_term_size = strlen(the_term);
	char *vocab_loc_ptr;

	gettimeofday(&stats.start, NULL);
	//
	// (1) search the first level, the headers
	//
	low = 0;
	high = node_count -1;
	while (low <= high) {
		mid_header = (low + high) / 2;
		if (strcmp(headers_array[mid_header].term, the_term) > 0) {
			high = mid_header - 1;
		} else if (strcmp(headers_array[mid_header].term, the_term) < 0) {
			low = mid_header + 1;
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
	if (strcmp(headers_array[mid_header].term, the_term) > 0) {
		mid_header = mid_header - 1;
	}
	gettimeofday(&stats.end, NULL);
	add_search_time();

	//
	// (3) search the second level
	//

	//
	// (3-1) read the node from the disk
	//
	gettimeofday(&stats.start, NULL);
#if (USE_O_DIRECT == 1)
	lseek(dict_file_odirect, headers_array[mid_header].node_ptr, SEEK_SET);
	read(dict_file_odirect, aligned_buffer, node_max_size);
	//int ret = read(dict_file_odirect, aligned_buffer, node_max_size);
	//printf("ret (read bytes): %d\n", ret);
	node_buffer = (char *)aligned_buffer;
#else
	fseek(dict_file, headers_array[mid_header].node_ptr, SEEK_SET);
	fread(node_buffer, node_max_size, 1, dict_file);
#endif
	gettimeofday(&stats.end, NULL);
	add_io_time();

	add_bytes_read(node_max_size);

	//
	// (3-2) find node length and where vocab_loc starts
	//
	gettimeofday(&stats.start, NULL);
	node_length_ptr = (node_length_t *)node_buffer;
	node_ptr = node_buffer + sizeof(node_length_t);

	//
	// (3-3) binary search in the node to locate the block and then iterative search in the block
	//
#if 1
	low = 0;
	high = *node_length_ptr - 1;
	while (low <= high) {
		mid = (low + high) / 2;
		vocab_loc_ptr = node_ptr + mid * (sizeof(vocab_loc_t) + block_size * POSTING_PTR_SIZE);
		term_ptr = node_buffer + *(vocab_loc_t *)vocab_loc_ptr;
		n = *(uint8_t *)term_ptr;
		if (strncmp(&term_ptr[1], the_term, n) > 0) {
			high = mid - 1;
		} else if (strncmp(&term_ptr[1], the_term, n) < 0) {
			low = mid + 1;
		} else {
			// only the first n terms matched, need to check
			// the length for matching
			if (n < the_term_size) {
				low = mid + 1;
			} else if (n > the_term_size) {
				high = mid - 1;
			} else if (the_term_size == n) {
				gettimeofday(&stats.end, NULL);
				add_search_time();
				return 1;
				//break;
			}
		}
	}

	// check if the term is in the previous block
	if ((strncmp(&term_ptr[1], the_term, n) > 0)) {
		mid = mid - 1;
		vocab_loc_ptr = node_ptr + mid * (sizeof(vocab_loc_t) + block_size * POSTING_PTR_SIZE);
		term_ptr = node_buffer + *(vocab_loc_t *)vocab_loc_ptr;
	}
	// the last header and the last block
	if ((mid_header == (node_count - 1)) && ( mid == (*node_length_ptr - 1))) {
		// *(vocab_loc *)(buffer + sizeof(node_loc_t)) is the first vocab pointer in the node.
		// the first vocab points to the beginning of the terms.
		// then find out how many postings pointers there are, the number of postings
		// points is the same as the block size.
		real_block_size = (node_buffer + *(vocab_loc_t *) (node_buffer + sizeof(node_length_t)))
								- (vocab_loc_ptr + sizeof(vocab_loc_t));
		real_block_size = real_block_size / POSTING_PTR_SIZE;
	} else {
		real_block_size = block_size;
	}

	// iterative search in the block
	for (w = 0; w < real_block_size; w++) {
		n = *term_ptr;
		term_ptr++;
		if (strncmp(term_ptr, the_term, n) == 0) {
			if (the_term_size == n) {
				gettimeofday(&stats.end, NULL);
				add_search_time();
				return 1;
			}
		}
		term_ptr += n;
	}

#else
	long long i;

	//
	// (3-3) iterative search in the node
	//
	for (i = 0; i < *node_length_ptr; i++) {
		// locate where terms starts
		term_ptr = node_buffer + *((vocab_loc_t *)node_ptr);

		if (i == (*node_length_ptr - 1)) {
			// *(vocab_loc *)(buffer + sizeof(node_loc_t)) is the first vocab pointer in the node.
			// the first vocab points to the beginning of the terms.
			// then find out how many postings pointers there are, the number of postings
			// points is the same as the block size.
			real_block_size = ((node_buffer + *(vocab_loc_t *) (node_buffer + sizeof(node_length_t))) - (node_ptr + sizeof(vocab_loc_t)));
			real_block_size = real_block_size / POSTING_PTR_SIZE;
		} else {
			real_block_size = block_size;
		}

		for (w = 0; w < real_block_size; w++) {
			n = *term_ptr;
			term_ptr++;
			if (strncmp(term_ptr, the_term, n) == 0) {
				if (the_term_size == n) {
					return 1;
				}
			}
			term_ptr += n;
		}

		node_ptr += sizeof(vocab_loc_t) + real_block_size * POSTING_PTR_SIZE;

	}
#endif

	gettimeofday(&stats.end, NULL);
	add_search_time();
	return 0;
}

static char *all_nodes = NULL;

void search_blocked_inmemory_init() {
	char *buffer;
	long long i, buffer_max_size;

	//
	// (1) open the dict file
	//
	dict_file = fopen(DICT_BLOCKED_FILENAME, "rb");
	if (!dict_file) {
		fprintf(stderr, "ERROR open file %s\n", DICT_BLOCKED_FILENAME);
		exit(2);
	}

	//
	// (2) read header terms from file
	//
	fread(&node_count, sizeof(node_count), 1, dict_file);
	dbg_printf("node_count: %d\n", node_count);
	// header term has a maximum length of MAX_WORD_LENGTH + 1
	buffer_max_size = node_count * (MAX_WORD_LENGTH + 1 + sizeof(node_ptr_t));
	buffer = (char *) malloc(buffer_max_size);
	//buffer_max_size is the upper bound for the headers
	fread(buffer, buffer_max_size, 1, dict_file);

	//
	// (3) convert into proper header structures
	//
	headers_array = (dict_blocked_t *) malloc(node_count * sizeof(dict_blocked_t));
	char *buffer_ptr = buffer;
	for (i = 0; i < node_count; i++) {
		strncpy(headers_array[i].term, buffer_ptr, strlen(buffer_ptr) + 1);
		buffer_ptr += strlen(buffer_ptr) + 1;
		memcpy(&headers_array[i].node_ptr, buffer_ptr, sizeof(node_ptr_t));
		buffer_ptr += sizeof(node_ptr_t);
	}
	free(buffer);

#if 0
	for (i = 0; i < node_count; i++) {
		printf("header[%lld]: %s\n", i, headers_array[i].term);
	}
	exit(2);
#endif

	//
	// (4) read all nodes in memory
	//
	node_max_size = hd_sector_size * num_of_sectors;
	all_nodes = (char *)malloc(node_max_size * node_count);
	if (!all_nodes) {
		perror("ERROR: malloc for all_nodes\n");
		exit(2);
	}
	// reset the fd to the first node in the file
	fseek(dict_file, headers_array[0].node_ptr, SEEK_SET);
	fread(all_nodes, node_max_size, node_count, dict_file);

	stats.io_time = 0;
	stats.search_time = 0;
	stats.bytes_read = 0;
}

int search_blocked_inmemory(char *the_term) {
	long long low, high, mid_header, mid, w, n;
	long long the_term_size = strlen(the_term);
	char *vocab_loc_ptr;

	gettimeofday(&stats.start, NULL);
	//
	// (1) search the first level, the headers
	//
	low = 0;
	high = node_count -1;
	while (low <= high) {
		mid_header = (low + high) / 2;
		if (strcmp(headers_array[mid_header].term, the_term) > 0) {
			high = mid_header - 1;
		} else if (strcmp(headers_array[mid_header].term, the_term) < 0) {
			low = mid_header + 1;
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
	if (strcmp(headers_array[mid_header].term, the_term) > 0) {
		mid_header = mid_header - 1;
	}
	gettimeofday(&stats.end, NULL);
	add_search_time();

	//
	// (3) search the second level
	//
	gettimeofday(&stats.start, NULL);

	//
	// (3-1) locate the node in memory
	//
	node_buffer = all_nodes + (node_max_size * mid_header);

	//
	// (3-2) find node length and where vocab_loc starts
	//
	node_length_ptr = (node_length_t *)node_buffer;
	node_ptr = node_buffer + sizeof(node_length_t);

	//
	// (3-3) binary search in the node to locate the block and then iterative search in the block
	//
	low = 0;
	high = *node_length_ptr - 1;
	while (low <= high) {
		mid = (low + high) / 2;
		vocab_loc_ptr = node_ptr + mid * (sizeof(vocab_loc_t) + block_size * POSTING_PTR_SIZE);
		term_ptr = node_buffer + *(vocab_loc_t *)vocab_loc_ptr;
		n = *(uint8_t *)term_ptr;
		if (strncmp(&term_ptr[1], the_term, n) > 0) {
			high = mid - 1;
		} else if (strncmp(&term_ptr[1], the_term, n) < 0) {
			low = mid + 1;
		} else {
			// only the first n terms matched, need to check
			// the length for matching
			if (n < the_term_size) {
				low = mid + 1;
			} else if (n > the_term_size) {
				high = mid - 1;
			} else if (the_term_size == n) {
				gettimeofday(&stats.end, NULL);
				add_search_time();
				return 1;
				//break;
			}
		}
	}

	// check if the term is in the previous block
	if ((strncmp(&term_ptr[1], the_term, n) > 0)) {
		mid = mid - 1;
		vocab_loc_ptr = node_ptr + mid * (sizeof(vocab_loc_t) + block_size * POSTING_PTR_SIZE);
		term_ptr = node_buffer + *(vocab_loc_t *)vocab_loc_ptr;
	}
	// the last header and the last block
	if ((mid_header == (node_count - 1)) && ( mid == (*node_length_ptr - 1))) {
		// *(vocab_loc *)(buffer + sizeof(node_loc_t)) is the first vocab pointer in the node.
		// the first vocab points to the beginning of the terms.
		// then find out how many postings pointers there are, the number of postings
		// points is the same as the block size.
		real_block_size = (node_buffer + *(vocab_loc_t *) (node_buffer + sizeof(node_length_t)))
								- (vocab_loc_ptr + sizeof(vocab_loc_t));
		real_block_size = real_block_size / POSTING_PTR_SIZE;
	} else {
		real_block_size = block_size;
	}

	// iterative search in the block
	for (w = 0; w < real_block_size; w++) {
		n = *term_ptr;
		term_ptr++;
		if (strncmp(term_ptr, the_term, n) == 0) {
			if (the_term_size == n) {
				gettimeofday(&stats.end, NULL);
				add_search_time();
				return 1;
			}
		}
		term_ptr += n;
	}

	gettimeofday(&stats.end, NULL);
	add_search_time();
	return 0;
}

