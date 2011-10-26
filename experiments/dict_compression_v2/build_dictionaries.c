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
#include "common.h"

void usage(char *prog) {
	printf("Usage:%s [--build | --serialise] --type [types]\n", basename(prog));
	printf("           [--build | -b]  the build operation (default)\n");
	printf("       [--serialise | -s]  the build operation (default)\n");
	printf("            [--type | -t]  [all, fixed, string, blocked, embed, embedfront, embedfixed, embedfixed-null, embedvar, orig]\n");
	printf(" [--num-of-sectors | -ns]  number of sectors to be aligned\n");
	printf("     [--block-size | -bs]  the block size\n");
	exit(1);
}


void serialise_orig() {
	char term[MAX_WORD_LENGTH+1];
	FILE *serialised_file = NULL;
	FILE *vocab_file = NULL;
	long long term_count = 0;
	long long i;

	printf("serialising orig......\n");

	//
	// (1) read term_count
	//
	vocab_file = fopen(VOCAB_FILENAME, "rb");
	serialised_file = fopen(SERIALISE_ORIG_FILENAME, "wb");
	fread(&term_count, sizeof(term_count), 1, vocab_file);
	dbg_printf("term_count: %lld\n", term_count);

	//
	// (2) write terms to file
	//
	for (i = 0; i < term_count; i++) {
		fread(term, MAX_WORD_LENGTH+1, 1, vocab_file);
		fwrite(term, strlen(term), 1, serialised_file);
		fwrite("\n", 1, 1, serialised_file);
	}

	fclose(vocab_file);
	fclose(serialised_file);
	printf("FINISHED\n");
}


int main(int argc, char *argv[]) {

	dict_type_t dict_type = NONE;
	operation_t operation = BUILD_OPT;

	if (argc == 1) {
		usage(argv[0]);
	}

	probe_hd_sector_size();

	for (int param = 1; param < argc; param++) {
		if ((strcmp(argv[param], "--serialise") == 0) || (strcmp(argv[param], "-s") == 0)) {
			operation = SERIALISE_OPT;
		} else  if ((strcmp(argv[param], "--build") == 0) ||(strcmp(argv[param], "-b") == 0)) {
			operation = BUILD_OPT;
		} else  if ((strcmp(argv[param], "--num-of-sectors") == 0) ||(strcmp(argv[param], "-ns") == 0)) {
			num_of_sectors = atoi(argv[++param]);
			dbg_printf("num_of_sectors: %d\n", num_of_sectors);
		} else  if ((strcmp(argv[param], "--block-size") == 0) ||(strcmp(argv[param], "-bs") == 0)) {
			block_size = atoi(argv[++param]);
			dbg_printf("block_size: %d\n", block_size);
		} else if ((strcmp(argv[param], "--type") == 0) || (strcmp(argv[param], "-t") == 0)) {
			char *type = argv[++param];
			if (type == NULL) {
				usage(argv[0]);
			}
			dbg_printf("dict type: %s\n", type);
			if (strcmp(type, "all") == 0) {
				dict_type = ALL;
			} else if (strcmp(type, "fixed") == 0) {
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
			} else if (strcmp(type, "embedvar") == 0) {
				dict_type = EMBEDVAR;
			} else if (strcmp(type, "embedvar_v2") == 0) {
				dict_type = EMBEDVAR_V2;
			} else if (strcmp(type, "orig") == 0) {
				dict_type = ORIG;
			} else {
				printf("ERROR: no support for %s\n", type);
				usage(argv[0]);
			}
		} else {
			usage(argv[0]);
		}
	}

	printf("      sector size: %lu\n", hd_sector_size);
	printf("number of sectors: %d\n", num_of_sectors);
	printf("   the block size: %d\n", block_size);

	if (operation == BUILD_OPT) {
		if ((dict_type == FIXED) || (dict_type == ALL)) {
			build_fixed();
		}
		if ((dict_type == STRING) || (dict_type == ALL)) {
			build_string();
		}
		if ((dict_type == BLOCKED) || (dict_type == ALL)) {
			build_blocked();
		}
		if ((dict_type == EMBED) || (dict_type == ALL)) {
			build_embed();
		}
		if ((dict_type == EMBEDFRONT) || (dict_type == ALL)) {
			build_embedfront();
		}
		if ((dict_type == EMBEDFIXED) || (dict_type == ALL)) {
			build_embedfixed();
		}
		if ((dict_type == EMBEDFIXED_NULL) || (dict_type == ALL)) {
			build_embedfixed_null();
		}
		//if ((dict_type == EMBEDVAR) || (dict_type == ALL)) {
		//	build_embedvar();
		//}
		//if ((dict_type == EMBEDVAR_V2) || (dict_type == ALL)) {
		//	build_embedvar_v2();
		//}
	// for testing purpose
	} else if (operation == SERIALISE_OPT) {
		if ((dict_type == FIXED) || (dict_type == ALL)) {
			serialise_fixed();
		}
		if ((dict_type == STRING) || (dict_type == ALL)) {
			serialise_string();
		}
		if ((dict_type == BLOCKED) || (dict_type == ALL)) {
			serialise_blocked();
		}
		if ((dict_type == EMBED) || (dict_type == ALL)) {
			serialise_embed();
		}
		if ((dict_type == EMBEDFRONT) || (dict_type == ALL)) {
			serialise_embedfront();
		}
		if ((dict_type == EMBEDFIXED) || (dict_type == ALL)) {
			serialise_embedfixed();
		}
		if ((dict_type == EMBEDFIXED_NULL) || (dict_type == ALL)) {
			serialise_embedfixed_null();
		}
		//if ((dict_type == EMBEDVAR) || (dict_type == ALL)) {
		//	serialise_embedvar();
		//}
		//if ((dict_type == EMBEDVAR_V2) || (dict_type == ALL)) {
		//	serialise_embedvar_v2();
		//}
		if ((dict_type == ORIG) || (dict_type == ALL)) {
			serialise_orig();
		}
	}
	return 0;
}
