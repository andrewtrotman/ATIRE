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
static Linked_list<dict_embedfront_t *> *headers_list;
static Linked_list<char *> *nodes_list;
static uint32_t node_count = 0;
static node_length_t node_length = 0;
static uint8_t common_prefix_count, suffix_count;
static char term[MAX_WORD_LENGTH+1];

void find_common_prefix(char *head_term, char *term, uint8_t *cp_count);

void build_embedfront() {
	char one_byte = '\0';
	char previous_term[MAX_WORD_LENGTH+1];
	long long i;
	dict_embedfront_t *one_header;
	char *node_buffer = NULL, *postings_buffer = NULL, *term_buffer = NULL;
	char *node_ptr = NULL, *postings_ptr = NULL, *term_ptr = NULL;
	long long node_max_size;
	long long needed_size, used_size;

	if (block_size <= 0) {
		fprintf(stderr, "ERROR: block size cannot be zero or less\n");
		exit(2);
	}

	//
	// (1) find the maximum size for the node and create temporary buffers
	//
	node_max_size = hd_sector_size * num_of_sectors;
	printf("    node_max_size: %lld\n", node_max_size);

	postings_buffer = (char *)malloc(node_max_size);
	memset(postings_buffer, 0, node_max_size);
	term_buffer = (char *)malloc(node_max_size);

	printf("building embedfront......\n");

	//
	// (2) create lists for headers and nodes
	//
	headers_list = new Linked_list<dict_embedfront_t *>();
	nodes_list = new Linked_list<char *> ();

	//
	// (3) read term_count
	//
	vocab_file = fopen(VOCAB_FILENAME, "rb");
	dict_file = fopen(DICT_EMBEDFRONT_FILENAME, "wb");
	fread(&term_count, sizeof(term_count), 1, vocab_file);
	dbg_printf("term_count: %lld\n", term_count);

	//
	// (4) reall all terms, build up the headers and nodes
	//
	i = 0;
	char filled_previously = TRUE;
	char new_node = TRUE;
	char node_is_full = FALSE, the_end = FALSE;
	node_count = 0;
	long long total_wasted = 0;
	long long terms_start_at = 0;
	term[0] = previous_term[0] = '\0';
	while (true) {
		//
		// (4-1) make sure the previous term is filled in properly before read the next term from file
		//
		if (filled_previously) {
			strcpy(previous_term, term);
			fread(term, MAX_WORD_LENGTH+1, 1, vocab_file);
			i++;
			if (i == term_count) {
				the_end = TRUE;
			}
		}

		//
		// (4-2) create a new header and the new associated node
		//
		if (new_node) {
			//one_header = (dict_embed_t *) malloc(sizeof(*one_header));
			one_header = new dict_embedfront_t;
			strncpy(one_header->term, term, strlen(term) + 1);
			headers_list->append(one_header);
			node_count++;

			//node_buffer =  (char *)malloc(sizeof(*node_buffer)*max_node_size*512);
			node_buffer = new char[node_max_size];
			nodes_list ->append(node_buffer);
			node_ptr = node_buffer;
			postings_ptr = postings_buffer;
			term_ptr = term_buffer;
			node_length = 0;

			new_node = FALSE;
			node_is_full = FALSE;
			previous_term[0] = '\0';
		}

		//
		// (4-3) fill as many terms as possible in the node
		//
		// only need to store the prefix_count for the first term in the node
		if (previous_term[0] == '\0') {
			suffix_count = strlen(term);
			needed_size = (long long)POSTING_PTR_SIZE + sizeof(suffix_count) + (sizeof(char) * suffix_count);
		} else {
			find_common_prefix(previous_term, term, &common_prefix_count);
			suffix_count = strlen(term) - common_prefix_count;
			needed_size = (long long)POSTING_PTR_SIZE + sizeof(common_prefix_count) + sizeof(suffix_count) + (sizeof(char) * suffix_count);
		}
		// the node length is stored at the last, so need to consider there is enough space left for it
		needed_size += sizeof(node_length_t);
		// find out how much space is already used in the node
		used_size = (postings_ptr - postings_buffer) + (term_ptr - term_buffer);
		if ((used_size + needed_size) < node_max_size) {
			// fill the postings buffer
			postings_ptr += POSTING_PTR_SIZE;

			// fill the term buffer
			if (previous_term[0] == '\0') {
				*term_ptr = suffix_count;
				term_ptr++;
				memcpy(term_ptr, term, suffix_count);
				term_ptr += suffix_count;
			} else {
				*term_ptr = common_prefix_count;
				term_ptr++;
				*term_ptr = suffix_count;
				term_ptr++;
				memcpy(term_ptr, &term[common_prefix_count], suffix_count);
				term_ptr += suffix_count;
			}

			node_length++;
			filled_previously = TRUE;
		} else {
			node_is_full = TRUE;
			filled_previously = FALSE;
			// the current block is full, need to create a new one
			new_node = TRUE;
		}

		//
		// (4-4) the current node is full or no more term to be processed
		//
		// if node_is_full and the_end happen at the same time, then the last block will not be handled.
		// need to take one more iteration to create a new node and full the buffer and then exit.
		if (node_is_full || the_end) {

			// store the postings in the node
			memcpy(node_ptr, postings_buffer, postings_ptr - postings_buffer);
			node_ptr += postings_ptr - postings_buffer;

			// store the terms in the node
			terms_start_at = node_ptr - node_buffer;
			memcpy(node_ptr, term_buffer, term_ptr - term_buffer);
			node_ptr += term_ptr - term_buffer;

			// find out how much space wasted
			total_wasted = node_max_size - (node_ptr - node_buffer) - sizeof (node_length_t);

			// store node length at the very end of the node
			node_ptr = node_buffer + node_max_size - sizeof(node_length_t);
			memcpy(node_ptr, &node_length, sizeof(node_length_t));
		}

#if 0
		if (node_is_full || the_end) {
			char term[MAX_WORD_LENGTH+1];
			char *node_ptr = node_buffer, *term_ptr;
			node_length_t node_length_02;
			long long i, j;
			uint8_t size;
			node_length_02 = *(node_length_t *) (node_buffer + node_max_size - sizeof(node_length_t));
			printf("node_legnth: %d, node_legnth_02: %d\n", node_length, node_length_02);
			term_ptr = node_buffer + node_length_02 * POSTING_PTR_SIZE;
			for (i = 0; i < node_length_02; i++) {
				if (i == 0) {
					suffix_count = *(uint8_t *)term_ptr;
					term_ptr++;
					memcpy(term, term_ptr, suffix_count);
					term[suffix_count] = '\0';
					term_ptr += suffix_count;
				} else {
					common_prefix_count = *(uint8_t *)term_ptr;
					term_ptr++;
					suffix_count = *(uint8_t *)term_ptr;
					term_ptr++;
					memcpy(&term[common_prefix_count], term_ptr, suffix_count);
					term[common_prefix_count+suffix_count] = '\0';
					term_ptr += suffix_count;
				}
				printf("term: %s\n", term);
			}
		}
#endif

		if ((the_end) && (filled_previously)) {
			break;
		}

	} //end of while (true)

	//
	// (5) write node_count and headers to disk
	//
	dbg_printf("node_count: %u\n", node_count);
	dict_embedfront_t *h = NULL;
	fwrite(&node_count, sizeof(node_count), 1, dict_file);
	for (h = headers_list->first(); h != NULL; h = headers_list->next()) {
		fwrite(h->term, strlen(h->term)+1, 1, dict_file);
		fwrite(&(h->node_ptr), sizeof(h->node_ptr), 1, dict_file);
	}

	//
	// (6) fill in the gap to make sure nodes starting at the beginning of a sector
	//
	//long long padding = hd_sector_size - ftell(dict_file) % hd_sector_size;
	long long padding = node_max_size - ftell(dict_file) % node_max_size;
	total_wasted += padding;
	for (i = 0; i < padding; i++) {
		fwrite(&one_byte, 1, 1, dict_file);
	}

	//
	// (7) write nodes to disk and update node pointer in headers
	//
	char *n = NULL;
	for (n = nodes_list->first(), h = headers_list->first(); n != NULL; n = nodes_list->next(), h = headers_list->next()) {
		h->node_ptr = ftell(dict_file);
		fwrite(n, node_max_size, 1, dict_file);
	}

	//
	// (8) re-write node_count and headers to disk
	//
	fseek(dict_file, 0, SEEK_SET);
	fwrite(&node_count, sizeof(node_count), 1, dict_file);
	for (h = headers_list->first(); h != NULL; h = headers_list->next()) {
		fwrite(h->term, strlen(h->term) + 1, 1, dict_file);
		fwrite(&(h->node_ptr), sizeof(h->node_ptr), 1, dict_file);
	}

	//
	// (9) finished
	//
	printf("     total_wasted: %lld bytes\n", total_wasted);
	fseek(dict_file, 0, SEEK_END);
	printf("  total file size: %ld\n", ftell(dict_file));
	fclose(dict_file);
	free(postings_buffer);
	free(term_buffer);
	delete headers_list;
	delete nodes_list;
	printf("FINISHED\n\n");
}


void find_common_prefix(char *head_term, char *term, uint8_t *cp_count) {
	long head_size = strlen(head_term), term_size = strlen(term);
	for (*cp_count = 0; (*cp_count < head_size) && (*cp_count < term_size); (*cp_count)++) {
		//printf("cp_count: %d\n", *cp_count);
		if (head_term[*cp_count] != term[*cp_count]) {
			return;
		}
	}
}

static dict_embedfront_t *headers_array;

void serialise_embedfront() {
	FILE *serialised_file = NULL;
	char *buffer;
	long long i, j, buffer_max_size;
	char term[MAX_WORD_LENGTH+1];

	printf("\nserialising embed......");

	//
	// (1) open files
	//
	dict_file = fopen(DICT_EMBEDFRONT_FILENAME, "rb");
	if (!dict_file) { fprintf(stderr, "ERROR open file %s\n", DICT_EMBEDFRONT_FILENAME); exit(2); }
	serialised_file = fopen(SERIALISE_EMBEDFRONT_FILENAME, "wb");
	if (!serialised_file) { fprintf(stderr, "ERROR open file %s\n", SERIALISE_EMBEDFRONT_FILENAME); exit(2); }

	//
	// (2) read header terms from file
	//
	fread(&node_count, sizeof(node_count), 1, dict_file);
	dbg_printf("node_count: %d\n", node_count);
	// header term has a maximum length of MAX_WORD_LENGTH + 1
	buffer_max_size = node_count * (MAX_WORD_LENGTH + 1 + sizeof(node_ptr_t));
	buffer = (char *)malloc(buffer_max_size);
	fread(buffer, buffer_max_size, 1, dict_file);

	//
	// (3) convert into proper header structures
	//
	headers_array = (dict_embedfront_t *)malloc(node_count * sizeof(dict_embedfront_t));
	char *buffer_ptr = buffer;
	for (i = 0; i < node_count; i++) {
		strncpy(headers_array[i].term, buffer_ptr, strlen(buffer_ptr)+1);
		buffer_ptr += strlen(buffer_ptr)+1;
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
	// (4) read one node at a time, and serialise the terms in the node
	//
	buffer_max_size = hd_sector_size * num_of_sectors;
	buffer = (char *)malloc(buffer_max_size);
	char *term_ptr;
	for (i = 0; i < node_count; i++) {
		fseek(dict_file, headers_array[i].node_ptr, SEEK_SET);
		fread(buffer, buffer_max_size, 1, dict_file);

		// find out the node length which is stored at the very end of the node
		buffer_ptr = buffer + buffer_max_size - sizeof(node_length_t);
		memcpy(&node_length, buffer_ptr, sizeof(node_length_t));

		term_ptr = buffer + node_length * POSTING_PTR_SIZE;
		for (j = 0; j < node_length; j++) {
			if (j == 0) {
				suffix_count = *(uint8_t *)term_ptr;
				term_ptr++;
				memcpy(term, term_ptr, suffix_count);
				term[suffix_count] = '\0';
				term_ptr += suffix_count;
			} else {
				common_prefix_count = *(uint8_t *)term_ptr;
				term_ptr++;
				suffix_count = *(uint8_t *)term_ptr;
				term_ptr++;
				memcpy(&term[common_prefix_count], term_ptr, suffix_count);
				term[common_prefix_count+suffix_count] = '\0';
				term_ptr += suffix_count;
			}
			fwrite(term, strlen(term), 1, serialised_file);
			fwrite("\n", 1, 1, serialised_file);
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

static char *node_buffer, *node_ptr;
static long long node_max_size;
static node_length_t *node_length_ptr;
#if (USE_O_DIRECT == 1)
	static int dict_file_odirect;
	static void *aligned_buffer;
#endif

void search_embedfront_ondisk_init() {
	char *buffer;
	long long buffer_max_size, i;

	//
	// (1) open the dict
	//
	dict_file = fopen(DICT_EMBEDFRONT_FILENAME, "rb");
	if (!dict_file) {
		fprintf(stderr, "ERROR open file %s\n", DICT_EMBEDFRONT_FILENAME);
		exit(2);
	}


	//
	// (2) read node_count
	//
	fread(&node_count, sizeof(node_count), 1, dict_file);
	dbg_printf("node_count: %d\n", node_count);
	// header term has a maximum length of MAX_WORD_LENGTH + 1
	buffer_max_size = node_count * (MAX_WORD_LENGTH + 1 + sizeof(node_ptr_t));
	buffer = (char *)malloc(buffer_max_size);
	fread(buffer, buffer_max_size, 1, dict_file);

	//
	// (3) convert into proper header structures
	//
	headers_array = (dict_embedfront_t *) malloc(node_count * sizeof(dict_embedfront_t));
	char *buffer_ptr = buffer;
	for (i = 0; i < node_count; i++) {
		strncpy(headers_array[i].term, buffer_ptr, strlen(buffer_ptr) + 1);
		buffer_ptr += strlen(buffer_ptr) + 1;
		memcpy(&headers_array[i].node_ptr, buffer_ptr, sizeof(node_ptr_t));
		buffer_ptr += sizeof(node_ptr_t);
	}
	free(buffer);

	//
	// (4) pre-allocate a node for use
	//
	node_max_size = hd_sector_size * num_of_sectors;
	node_buffer = (char *)malloc(node_max_size);

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

	if ((dict_file_odirect = open(DICT_EMBEDFRONT_FILENAME, O_RDONLY | O_DIRECT)) , 0) {
		fprintf(stderr, "open with O_DIRECT for %s\n", DICT_EMBEDFRONT_FILENAME);
		exit(2);
	}
#endif
}


int search_embedfront_ondisk(char *the_term) {
	long long low, high, mid, i;
	char *term_ptr;

	gettimeofday(&stats.start, NULL);
	//
	// (1) search the first level, the headers
	//
	low = 0;
	high = node_count - 1;
	while (low <= high) {
		mid = (low + high) / 2;
		if (strcmp(headers_array[mid].term, the_term) > 0) {
			high = mid - 1;
		} else if (strcmp(headers_array[mid].term, the_term) < 0) {
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
#if (USE_O_DIRECT == 1)
	lseek(dict_file_odirect, headers_array[mid].node_ptr, SEEK_SET);
	read(dict_file_odirect, aligned_buffer, node_max_size);
	//int ret = read(dict_file_odirect, aligned_buffer, node_max_size);
	//printf("ret (read bytes): %d\n", ret);
	node_buffer = (char *)aligned_buffer;
#else
	fseek(dict_file, headers_array[mid].node_ptr, SEEK_SET);
	fread(node_buffer, node_max_size, 1, dict_file);
#endif
	gettimeofday(&stats.end, NULL);
	add_io_time();

	add_bytes_read(node_max_size);

	gettimeofday(&stats.start, NULL);
	// find node length
	node_ptr = node_buffer + node_max_size - sizeof(node_length_t);
	node_length_ptr = (node_length_t *)node_ptr;

	// locate where terms start
	term_ptr = node_buffer + *node_length_ptr * POSTING_PTR_SIZE;

	// compare the first term
	suffix_count = *(uint8_t *)term_ptr;
	term_ptr++;
	strncpy(term, term_ptr, suffix_count);
	term[suffix_count] = '\0';
	if (strcmp(term, the_term) == 0) {
		gettimeofday(&stats.end, NULL);
		add_search_time();
		return 1;
	}
	term_ptr += suffix_count;

	// compare the rest of the terms
	for (i = 1; i < *node_length_ptr; i++) {
		common_prefix_count = *(uint8_t *)term_ptr;
		term_ptr++;
		suffix_count = *(uint8_t *)term_ptr;
		term_ptr++;

		// re-construct the current term
		strncpy(&term[common_prefix_count], term_ptr, suffix_count);
		term[common_prefix_count + suffix_count] = '\0';
		if (strcmp(term, the_term) == 0) {
			gettimeofday(&stats.end, NULL);
			add_search_time();
			return 1;
		}

		// points to the beginning of the next term
		term_ptr += suffix_count;
	}

	gettimeofday(&stats.end, NULL);
	add_search_time();
	return 0;
}

static char *all_nodes = NULL;

void search_embedfront_inmemory_init() {
	char *buffer;
	long long buffer_max_size, i;

	//
	// (1) open the dict
	//
	dict_file = fopen(DICT_EMBEDFRONT_FILENAME, "rb");
	if (!dict_file) {
		fprintf(stderr, "ERROR open file %s\n", DICT_EMBEDFRONT_FILENAME);
		exit(2);
	}


	//
	// (2) read node_count
	//
	fread(&node_count, sizeof(node_count), 1, dict_file);
	dbg_printf("node_count: %d\n", node_count);
	// header term has a maximum length of MAX_WORD_LENGTH + 1
	buffer_max_size = node_count * (MAX_WORD_LENGTH + 1 + sizeof(node_ptr_t));
	buffer = (char *)malloc(buffer_max_size);
	fread(buffer, buffer_max_size, 1, dict_file);

	//
	// (3) convert into proper header structures
	//
	headers_array = (dict_embedfront_t *) malloc(node_count * sizeof(dict_embedfront_t));
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
		printf("(%lld): %s\n", i, headers_array[i].term);
	}
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


int search_embedfront_inmemory(char *the_term) {
	long long low, high, mid, i;
	char *term_ptr;

	gettimeofday(&stats.start, NULL);
	//
	// (1) search the first level, the headers
	//
	low = 0;
	high = node_count - 1;
	while (low <= high) {
		mid = (low + high) / 2;
		if (strcmp(headers_array[mid].term, the_term) > 0) {
			high = mid - 1;
		} else if (strcmp(headers_array[mid].term, the_term) < 0) {
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

	// locate the node in memory
	node_buffer = all_nodes + (node_max_size * mid);

	// find node length
	node_ptr = node_buffer + node_max_size - sizeof(node_length_t);
	node_length_ptr = (node_length_t *)node_ptr;

	// locate where terms start
	term_ptr = node_buffer + *node_length_ptr * POSTING_PTR_SIZE;

	// compare the first term
	suffix_count = *(uint8_t *)term_ptr;
	term_ptr++;
	strncpy(term, term_ptr, suffix_count);
	term[suffix_count] = '\0';
	if (strcmp(term, the_term) == 0) {
		gettimeofday(&stats.end, NULL);
		add_search_time();
		return 1;
	}
	term_ptr += suffix_count;

	// compare the rest of the terms
	for (i = 1; i < *node_length_ptr; i++) {
		common_prefix_count = *(uint8_t *)term_ptr;
		term_ptr++;
		suffix_count = *(uint8_t *)term_ptr;
		term_ptr++;

		// re-construct the current term
		strncpy(&term[common_prefix_count], term_ptr, suffix_count);
		term[common_prefix_count + suffix_count] = '\0';
		if (strcmp(term, the_term) == 0) {
			gettimeofday(&stats.end, NULL);
			add_search_time();
			return 1;
		}

		// points to the beginning of the next term
		term_ptr += suffix_count;
	}

	gettimeofday(&stats.end, NULL);
	add_search_time();
	return 0;
}


