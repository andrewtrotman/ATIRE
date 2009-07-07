#ifndef __GA_ANT_H__

/* 
 * This header stores as much of my extensions for ant.c as possible.
 * The only change to ant.c that should be needed it to add '#include "ga_ant.h"'
 * and to change calls to ant() as calls to ga_ant()
 *
 * I know doing this in a header file is ugly, but I want to be able to use definitions
 * from ant.c 
 */
#include <time.h>
#include <stdlib.h>
#include "ga.h"
#include "ga_stemmer.h"
#include "ga_function.h"
#include "vocab.h"

#define NUM_OF_GENERATIONS 200
#define POPULATION_SIZE 200

double perform_query(ANT_ANT_param_block *, ANT_search_engine *, char *, long long *, long, ANT_mean_average_precision *, ANT_stemmer *);
char **get_queries(long *query_count, ANT_ANT_param_block *params);

/*
  TRIE_TEST()
  -----------
  Codifies the words into the vocab, and then
  prints out the trie
*/
void trie_test(ANT_search_engine *search_engine) {
	//  No more document_count() ?!

	Vocab *trie = new Vocab(search_engine);
	//	trie->print_stats();
	//	trie->trim(200);
	trie->print();
	delete trie;
}

/*
	GA_ANT()
	--------
	We should take cli args from ANT_ANT_PARAM_BLOCk

    STEMMER_FILE is optional, and if it is non-null it should be the filename of a file that contains a
    stemmer per line. If it exists, then the GA will not be run, each stemmer will instead be run on the 
    queries.
*/
/*
  TODO: make more like ANT(...)
  TODO: be able to specify a stemmer.
*/
void ga_ant(ANT_search_engine *search_engine, ANT_mean_average_precision *map, ANT_ANT_param_block *params, char **document_list, char **answer_list, char *stemmer_file) {
    char *query;
	char **queries;
    long query_count, i;
	long *topic_ids;
    ANT_search_engine_forum *output = NULL;
    long have_assessments = params->assessments_filename == NULL ? FALSE : TRUE;

	GA *ga;
	GA_stemmer *stemmer = new GA_stemmer(search_engine);
	GA_individual *ind = new GA_individual();
	srand(time(NULL));

    // Still here in case we need output from a single stemmer
    if (params->output_forum == ANT_ANT_param_block::TREC)
        output = new ANT_search_engine_forum_TREC(params->output_filename, params->participant_id, params->run_name, "RelevantInContext");
    else if (params->output_forum == ANT_ANT_param_block::INEX)
        output = new ANT_search_engine_forum_INEX(params->output_filename, params->participant_id, params->run_name, "RelevantInContext");

    /*
      Store queries and their topic ids - For the GA, we need these for each run.
     */
    queries = get_queries(&query_count, params);

    topic_ids = (long *) malloc(sizeof *topic_ids * query_count);
    for (i = 0; i < query_count; i++) {
        strip_end_punc(queries[i]);
        if (!have_assessments)
            exit(printf("No assessments given.\n"));
        else
            {
                topic_ids[i] = atol(queries[i]);
                if ((query = strchr(queries[i], ' ')) == NULL)
                    exit(printf("Line %ld: Can't process query as badly formed:'%s'\n", i, queries[i]));
            }
    }

	if (stemmer_file) {
		int count = 0;
		char buffer[4096];
		FILE *f = fopen(stemmer_file, "r");
		while (fgets(buffer, 4096, f) != NULL) {
			if (buffer[0] != ' ')
				continue;
			double result = 0.0;
			ind->sload(buffer);
			stemmer->set_stemmer(ind);
			stemmer->print(stderr);
			for (i = 0; i < query_count; i++) {
				long long hits;
				result += perform_query(params, search_engine, queries[i], &hits, topic_ids[i], map, stemmer);
			}
			fprintf(stderr, "%d %f\n", count++, result / (double)query_count);
		}
	} else {
        ga = new GA(POPULATION_SIZE, 
                    new GA_function(perform_query, params, search_engine, query_count, queries, topic_ids, map, stemmer), search_engine);
		ga->run(NUM_OF_GENERATIONS);
	}
}

#endif /* __GA_ANT_H__ */
