/*
 *  Author: Xiangfei Jia, fei@cs.otago.ac.nz, xjianz@gmail.com
 * Created: 14-Nov-2010
 *
 *
 */

#ifdef __linux__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#ifdef __linux__
#include <sched.h>
#endif
#include "common.h"
#include "NEXI_ant.h"
#include "NEXI_term_ant.h"
#include "NEXI_term_iterator.h"
#include "str.h"

#define BUFFER_SIZE 1024*1024

void usage(char *prog) {
	printf("Usage:%s --dict-type [types] --search-type [types]\n", basename(prog));
	printf("      [--dict-type | -dt]  [fixed, string, blocked, embed, embedfront, embedfixed, embedfixed-null]\n");
	printf("    [--search-type | -st]  [ondisk, inmemory]\n");
	printf(" [--num-of-sectors | -ns]  number of sectors to be aligned\n");
	printf("     [--block-size | -bs]  the block size\n");
	printf("     [--query-file | -qf]  the query file\n");
	exit(1);
}

int search_ondisk(dict_type_t dict_type, char *the_term, double *spent);
int search_inmemory(dict_type_t dict_type, char *the_term, double *spent);


int main(int argc, char *argv[]) {

	dict_type_t dict_type = NONE;
	search_t search_type = ONDISK;
	char buffer[BUFFER_SIZE];
	int found;
	double total_time = 0, spent;
	char *term_ptr;
	FILE *query_file = stdin;
	long total_terms = 0;

#ifdef __linux__
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(1, &set); // runs on the fourth core
	if (sched_setaffinity(0, sizeof(cpu_set_t), &set) < 0) {
		perror("sched_setaffinity\n");
		exit(2);
	}
#endif

	if (argc == 1) {
		usage(argv[0]);
	}

	probe_hd_sector_size();

	for (int param = 1; param < argc; param++) {
		if ((strcmp(argv[param], "--num-of-sectors") == 0) || (strcmp(argv[param], "-ns") == 0)) {
			num_of_sectors = atoi(argv[++param]);
			dbg_printf("num_of_sectors: %d\n", num_of_sectors);
		} else if ((strcmp(argv[param], "--block-size") == 0) || (strcmp(argv[param], "-bs") == 0)) {
			block_size = atoi(argv[++param]);
			dbg_printf("block_size: %d\n", block_size);
		} else if ((strcmp(argv[param], "--dict-type") == 0) || (strcmp(argv[param], "-dt") == 0)) {
			char *type = argv[++param];
			if (type == NULL) {
				usage(argv[0]);
			}
			dbg_printf("dict type: %s\n", type);
			if (strcmp(type, "fixed") == 0) {
				dict_type = FIXED;
			} else if (strcmp(type, "string") == 0) {
				dict_type = STRING;
			} else if (strcmp(type, "blocked") == 0) {
				dict_type = BLOCKED;
			} else if (strcmp(type, "embed") == 0) {
				dict_type = EMBED;
			} else if (strcmp(type, "embedfront") == 0) {
				dict_type = EMBEDFRONT;
			} else if (strcmp(type, "embedfixed") == 0) {
				dict_type = EMBEDFIXED;
			} else if (strcmp(type, "embedfixed-null") == 0) {
				dict_type = EMBEDFIXED_NULL;
			} else {
				printf("ERROR: no support for %s\n", type);
				usage(argv[0]);
			}
		} else if ((strcmp(argv[param], "--search-type") == 0) || (strcmp(argv[param], "-st") == 0)) {
			char *type = argv[++param];
			if (type == NULL) {
				usage(argv[0]);
			}
			dbg_printf("search type: %s\n", type);
			if (strcmp(type, "ondisk") == 0) {
				search_type= ONDISK;
			} else if (strcmp(type, "inmemory") == 0) {
				search_type = INMEMORY;
			} else {
				printf("ERROR: no support for %s\n", type);
				usage(argv[0]);
			}
		} else if ((strcmp(argv[param], "--query-file") == 0) || (strcmp(argv[param], "-qf") == 0)) {
			char *file = argv[++param];
			if (file == NULL) {
				usage(argv[0]);
			}
			dbg_printf("query file: %s\n", file);
			query_file = fopen(file, "r");
			if (!query_file) { fprintf(stderr, "ERROR open query file %s\n", file); exit(2); }
		} else {
			usage(argv[0]);
		}
	}

	printf("      sector size: %lu\n", hd_sector_size);
	printf("number of sectors: %d\n", num_of_sectors);
	printf("   the block size: %d\n", block_size);
	printf("    node_max_size: %ld\n", hd_sector_size * num_of_sectors);

	if (dict_type == FIXED) {
		if (search_type == ONDISK) {
			search_fixed_ondisk_init();
		} else if (search_type == INMEMORY) {
			search_fixed_inmemory_init();
		}
	}
	if (dict_type == STRING) {
		if (search_type == ONDISK) {
			search_string_ondisk_init();
		} else if (search_type == INMEMORY) {
			search_string_inmemory_init();
		}
	}
	if (dict_type == BLOCKED) {
		if (search_type == ONDISK) {
			search_blocked_ondisk_init();
		} else if (search_type == INMEMORY) {
			search_blocked_inmemory_init();
		}
	}
	if (dict_type == EMBED) {
		if (search_type == ONDISK) {
			search_embed_ondisk_init();
		} else if (search_type == INMEMORY) {
			search_embed_inmemory_init();
		}
	}
	if (dict_type == EMBEDFRONT) {
		if (search_type == ONDISK) {
			search_embedfront_ondisk_init();
		} else if (search_type == INMEMORY) {
			search_embedfront_inmemory_init();
		}
	}
	if (dict_type == EMBEDFIXED) {
		if (search_type == ONDISK) {
			search_embedfixed_ondisk_init();
		} else if (search_type == INMEMORY) {
			search_embedfixed_inmemory_init();
		}
	}
	if (dict_type == EMBEDFIXED_NULL) {
		if (search_type == ONDISK) {
			search_embedfixed_null_ondisk_init();
		} else if (search_type == INMEMORY) {
			search_embedfixed_null_inmemory_init();
		}
	}

	stats.io_time = 0;
	stats.search_time = 0;

#if ((defined __linux__) && (FLUSH_CACHE_BEFORE_QUERY == 1) )
		FILE *drop_cache = fopen("/proc/sys/vm/drop_caches", "w");
		if (!drop_cache) { fprintf(stderr, "ERROR open the cache file\n"); exit(2); }
		int n = 3;
#endif

	while(fgets(buffer, BUFFER_SIZE, query_file)) {
		// flush the cache before each query
#if ((defined __linux__) && (FLUSH_CACHE_BEFORE_QUERY == 1) )
		if (fwrite(&n, sizeof(n), 1, drop_cache) <= 0) {
			fprintf(stderr, "ERROR write the cache file\n"); exit(2);
		}
#endif
		term_ptr = strtok(buffer, " .-'&\n");
		while (term_ptr != NULL) {
			found = 0;
			total_terms++;
			if (search_type == ONDISK) {
				found = search_ondisk(dict_type, term_ptr, &spent);
				total_time += spent;
			} else if (search_type == INMEMORY) {
				found = search_inmemory(dict_type, term_ptr, &spent);
				total_time += spent;
			}

			if (found) {
				printf("term: %s (found)\n", term_ptr);
			} else {
				printf("term: %s (not found)\n", term_ptr);
			}
			term_ptr = strtok(NULL, " .-'&\n");
		}
	}

	printf("    io time: %.3f milliseconds\n", (double)stats.io_time / 1000);
	printf("search time: %.3f milliseconds\n", (double)stats.search_time / 1000);
	printf(" total time: %.3f milliseconds\n", (double)(stats.io_time + stats.search_time) / 1000);
	printf(" bytes read: %lld bytes\n", stats.bytes_read);
	printf("total terms: %ld\n", total_terms);

#if ((defined __linux__) && (FLUSH_CACHE_BEFORE_QUERY == 1) )
	fclose(drop_cache);
#endif
	return 0;
}

int search_ondisk(dict_type_t dict_type, char *the_term, double *spent) {
	//dbg_printf("term: %s\n", the_term);
	int found = 0;
	struct timeval start, end;

	//dbg_printf("search ondisk\n");

	if (dict_type == FIXED) {
		gettimeofday(&start, NULL);
		found = search_fixed_ondisk(the_term);
		gettimeofday(&end, NULL);
	} else if (dict_type == STRING) {
		gettimeofday(&start, NULL);
		found = search_string_ondisk(the_term);
		gettimeofday(&end, NULL);
	} else if (dict_type == BLOCKED) {
		gettimeofday(&start, NULL);
		found = search_blocked_ondisk(the_term);
		gettimeofday(&end, NULL);
	} else if (dict_type == EMBED) {
		gettimeofday(&start, NULL);
		found = search_embed_ondisk(the_term);
		gettimeofday(&end, NULL);
	} else if (dict_type == EMBEDFRONT) {
		gettimeofday(&start, NULL);
		found = search_embedfront_ondisk(the_term);
		gettimeofday(&end, NULL);
	} else if (dict_type == EMBEDFIXED) {
		gettimeofday(&start, NULL);
		found = search_embedfixed_ondisk(the_term);
		gettimeofday(&end, NULL);
	} else if (dict_type == EMBEDFIXED_NULL) {
		gettimeofday(&start, NULL);
		found = search_embedfixed_null_ondisk(the_term);
		gettimeofday(&end, NULL);
	}

	// in milliseconds
	*spent = (((end.tv_sec * 1e6) + end.tv_usec) - ((start.tv_sec * 1e6) + start.tv_usec)) / 1e3;
	return found;
}


int search_inmemory(dict_type_t dict_type, char *the_term, double *spent) {
	//dbg_printf("term: %s\n", the_term);
	int found = 0;
	struct timeval start, end;

	//dbg_printf("search inmemory\n");

	if (dict_type == FIXED) {
		gettimeofday(&start, NULL);
		found = search_fixed_inmemory(the_term);
		gettimeofday(&end, NULL);
	} else if (dict_type == STRING) {
		gettimeofday(&start, NULL);
		found = search_string_inmemory(the_term);
		gettimeofday(&end, NULL);
	} else if (dict_type == BLOCKED) {
		gettimeofday(&start, NULL);
		found = search_blocked_inmemory(the_term);
		gettimeofday(&end, NULL);
	} else if (dict_type == EMBED) {
		gettimeofday(&start, NULL);
		found = search_embed_inmemory(the_term);
		gettimeofday(&end, NULL);
	} else if (dict_type == EMBEDFRONT) {
		gettimeofday(&start, NULL);
		found = search_embedfront_inmemory(the_term);
		gettimeofday(&end, NULL);
	} else if (dict_type == EMBEDFIXED) {
		gettimeofday(&start, NULL);
		found = search_embedfixed_inmemory(the_term);
		gettimeofday(&end, NULL);
	} else if (dict_type == EMBEDFIXED_NULL) {
		gettimeofday(&start, NULL);
		found = search_embedfixed_null_inmemory(the_term);
		gettimeofday(&end, NULL);
	}

	// in milliseconds
	*spent = (((end.tv_sec * 1e6) + end.tv_usec) - ((start.tv_sec * 1e6) + start.tv_usec)) / 1e3;
	return found;
}
