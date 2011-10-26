/*
 *  Author: Xiangfei Jia, fei@cs.otago.ac.nz, xjianz@gmail.com
 * Created: 11-Nov-2010
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "RB_tree.h"
#include "common.h"
#include "str.h"

#define BUFFER_SIZE MAX_WORD_LENGTH+1024*1024

void usage(char *prog) {
	printf("Usage:%s [--ant | --google]\n", basename(prog));
	printf("         [--ant | -a]      build vocab from the index\n");
	printf("         [--google | -g]   build vocab from google word list\n");
	printf("         [--test | -t]   for testing purpose\n");
	exit(1);
}

void ant_vocab() {
	char *term;
	ANT_memory memory;
	ANT_search_engine search_engine(&memory);
	ANT_btree_iterator iterator(&search_engine);
	FILE *vocab_file = NULL;
	char the_term[MAX_WORD_LENGTH+1];
	long long term_count = 0;

	printf("\nbuilding vocab from the index......\n");

	vocab_file = fopen(VOCAB_FILENAME, "wb");

	// reserve the space for term_count
	fwrite(&term_count, sizeof(term_count), 1, vocab_file);

	for (term = iterator.first(NULL); term != NULL; term = iterator.next()) {
		if (term[0] != '~') {
			strncpy(the_term, term, MAX_WORD_LENGTH);
			the_term[MAX_WORD_LENGTH] = '\0';
			fwrite(the_term, MAX_WORD_LENGTH+1, 1, vocab_file);
			//fwrite("\n", 1, 1, vocab_file);
			term_count++;
		}
	}
	printf("term_count: %lld\n", term_count);

	// write the correct term_count to the file
	fseek(vocab_file, 0L, SEEK_SET);
	fwrite(&term_count, sizeof(term_count), 1, vocab_file);

	fclose(vocab_file);
	printf("FINISHED\n");

}

struct cmpstr {
	int operator() (const term_t *a, const term_t *b) {
		return strcmp(a->term, b->term);
	}
};

int cmp(const void *a, const void *b) {
	term_t *t1 = (term_t *)a, *t2 = (term_t*)b;
	return strcmp(t1->term, t2->term);
}

void google_vocab() {
	char buffer[BUFFER_SIZE];
	char *end = NULL;
	FILE *google_vocab = NULL;
	FILE *vocab_file = NULL;
	long long term_count = 0, i, unique_term_count;
	term_t *terms_list, one_term;
	RB_tree<term_t *, cmpstr> *rb_tree;
	char found;

	printf("\nbuilding vocab from the Google word list......\n");

	//
	// (1) open files
	//
	if ((google_vocab = fopen("vocab", "rb")) == NULL) {
		perror("ERROR: openging file google vocab\n");
		exit(2);
	}
	if ((vocab_file = fopen(VOCAB_FILENAME, "wb")) == NULL) {
		perror("ERROR: openging file vocab\n");
		exit(2);
	}

	//
	// (2) count the number of terms
	//
	while(fgets(buffer, BUFFER_SIZE, google_vocab)) {
		term_count++;
	}
	dbg_printf("term_count: %lld\n", term_count);

	//
	// (3) alloc the list of terms, and fill the terms
	//
	terms_list = (term_t *)malloc(term_count*sizeof(*terms_list));
	//terms_list = new term_t[term_count];
	rb_tree = new RB_tree<term_t *, cmpstr>();
	fseek(google_vocab, 0, SEEK_SET);
	unique_term_count = 0;
	while (fgets(buffer, BUFFER_SIZE, google_vocab)) {
		//end = strnstr(buffer, "\t", BUFFER_SIZE);
		buffer[BUFFER_SIZE-1] = '\0';
		end = strstr(buffer, "\t");
		if (end == NULL) {
			fprintf(stderr, "\n\nERROR: empty term\n\n");
		} else {
			*end = '\0';
			buffer[MAX_WORD_LENGTH] = '\0';
			strlower(buffer);
			strcpy(one_term.term, buffer);
			found = rb_tree->search(&one_term);
			if (!found) {
				strcpy(terms_list[unique_term_count].term, buffer);
				rb_tree->insert(&terms_list[unique_term_count]);
				unique_term_count++;
			} else {
				//printf("found repeated: %s\n", buffer);
			}
		}
	}


	//
	// (4) write the number of unique terms
	//
	fwrite(&unique_term_count, sizeof(unique_term_count), 1, vocab_file);
	dbg_printf("term_count: %lld, unique_term_count: %lld\n", term_count, unique_term_count);

	//
	// (5) write terms to file
	//
	qsort(terms_list, unique_term_count, sizeof(term_t), cmp);
	for (i = 0; i < unique_term_count; i++) {
		fwrite(terms_list[i].term, MAX_WORD_LENGTH+1, 1, vocab_file);
	}

	//
	// (6) finished
	//
	free(terms_list);
	delete rb_tree;
	fclose(google_vocab);
	fclose(vocab_file);
	printf("FINISHED\n");
}


void test_vocab() {
	char buffer[BUFFER_SIZE];
	char *end = NULL;
	FILE *google_vocab = NULL;
	FILE *vocab_file = NULL;
	long long term_count = 0, i, unique_term_count;
	term_t *terms_list, one_term;
	RB_tree<term_t *, cmpstr> *rb_tree;
	char found;

	printf("\nbuilding vocab for test......\n");

	//
	// (1) open files
	//
	if ((google_vocab = fopen("vocab", "rb")) == NULL) {
		perror("ERROR: openging file google vocab\n");
		exit(2);
	}
	if ((vocab_file = fopen(VOCAB_FILENAME, "wb")) == NULL) {
		perror("ERROR: openging file vocab\n");
		exit(2);
	}

	//
	// (2) count the number of terms
	//
	while(fgets(buffer, BUFFER_SIZE, google_vocab)) {
		term_count++;
	}
	dbg_printf("term_count: %lld\n", term_count);

	//
	// (3) alloc the list of terms, and fill the terms
	//
	terms_list = (term_t *)malloc(term_count*sizeof(*terms_list));
	//terms_list = new term_t[term_count];
	rb_tree = new RB_tree<term_t *, cmpstr>();
	fseek(google_vocab, 0, SEEK_SET);
	unique_term_count = 0;
	while (fgets(buffer, BUFFER_SIZE, google_vocab)) {
		//end = strnstr(buffer, "\t", BUFFER_SIZE);
		buffer[BUFFER_SIZE-1] = '\0';
		end = strstr(buffer, "\n");
		if (end == NULL) {
			fprintf(stderr, "\n\nERROR: empty term\n\n");
		} else {
			*end = '\0';
			buffer[MAX_WORD_LENGTH] = '\0';
			strlower(buffer);
			strcpy(one_term.term, buffer);
			found = rb_tree->search(&one_term);
			if (!found) {
				strcpy(terms_list[unique_term_count].term, buffer);
				rb_tree->insert(&terms_list[unique_term_count]);
				unique_term_count++;
			} else {
				//printf("found repeated: %s\n", buffer);
			}
		}
	}


	//
	// (4) write the number of unique terms
	//
	fwrite(&unique_term_count, sizeof(unique_term_count), 1, vocab_file);
	dbg_printf("term_count: %lld, unique_term_count: %lld\n", term_count, unique_term_count);

	//
	// (5) write terms to file
	//
	qsort(terms_list, unique_term_count, sizeof(term_t), cmp);
	for (i = 0; i < unique_term_count; i++) {
		fwrite(terms_list[i].term, MAX_WORD_LENGTH+1, 1, vocab_file);
	}

	//
	// (6) finished
	//
	free(terms_list);
	delete rb_tree;
	fclose(google_vocab);
	fclose(vocab_file);
	printf("FINISHED\n");
}


int main(int argc, char *argv[]) {

	if (argc == 1) {
		usage(argv[0]);
	}

	for (int param = 1; param < argc; param++) {
		if ((strcmp(argv[param], "--ant") == 0) || (strcmp(argv[param], "-a") == 0)) {
			ant_vocab();
		} else if ((strcmp(argv[param], "--google") == 0) || (strcmp(argv[param], "-g") == 0)) {
			google_vocab();
		} else if ((strcmp(argv[param], "--test") == 0) || (strcmp(argv[param], "-t") == 0)) {
			test_vocab();
		} else {
			usage(argv[0]);
		}
	}

	return 0;
}
