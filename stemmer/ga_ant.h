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

/*
	PERFORM_QUERY_W_STEMMER()
	---------------
	Taken almost verbatim from PERFORM_QUERY()
*/
double perform_query_w_stemmer(ANT_ANT_param_block *params, ANT_search_engine *search_engine, char *query, long long *matching_documents, long topic_id = -1, ANT_mean_average_precision *map = NULL, ANT_stemmer *stemmer = NULL) {
	ANT_time_stats stats;
	long long now;
	long did_query;
	char token[1024];
	char *token_start, *token_end;
	long long hits;
	size_t token_length;
	ANT_search_engine_accumulator *ranked_list;
	double average_precision = 0.0;

	search_engine->stats_initialise();		// if we are command-line then report query by query stats

	did_query = FALSE;
	now = stats.start_timer();
	search_engine->init_accumulators();

	token_end = query;

	while (*token_end != '\0')
		{
			token_start = token_end;
			while (!ANT_isalnum(*token_start) && *token_start != '\0')
				token_start++;
			if (*token_start == '\0')
				break;
			token_end = token_start;
			while (ANT_isalnum(*token_end) || *token_end == '+')
				token_end++;
			strncpy(token, token_start, token_end - token_start);
			token[token_end - token_start] = '\0';
			token_length = token_end - token_start;
			//	strlwr(token);

			/*
			  process the next search term - either stemmed or not.
			*/
			if (stemmer == NULL)
				search_engine->process_one_search_term(token);
			else
				search_engine->process_one_stemmed_search_term(stemmer, token);

			did_query = TRUE;
		}

	/*
	  Rank the results list
	*/
	ranked_list = search_engine->sort_results_list(params->sort_top_k, &hits); // rank

	/*
	  Reporting
	*/
	if (params->stats & ANT_ANT_param_block::SHORT)
		{
			if (topic_id >= 0)
				printf("Topic:%ld ", topic_id);
			printf("Query '%s' found %lld documents ", query, hits);
			stats.print_time("(", stats.stop_timer(now), ")");
		}

	if (did_query && params->stats & ANT_ANT_param_block::QUERY)
		search_engine->stats_text_render();

	/*
	  Compute average previsions
	*/
	if (map != NULL)
		if (params->metric == ANT_ANT_param_block::MAP)
			average_precision = map->average_precision(topic_id, search_engine);
		else
			average_precision = map->average_generalised_precision(topic_id, search_engine);

	/*
	  Return the number of document that matched the user's query
	*/
	*matching_documents = hits;

	/*
	  Add the time it took to search to the global stats for the search engine
	*/
	search_engine->stats_add();
	/*
	  Return the precision
	*/
	return average_precision;
}

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
void ga_ant(char *topic_file, char *qrel_file, char *stemmer_file, long qrel_format) {
	ANT_relevant_document *assessments;
	char query[1024];
	long line, *topic_ids = NULL;
	long long documents_in_id_list, number_of_assessments;
	ANT_memory memory;
	FILE *fp;
	char *query_text, **document_list;
	char **all_queries = NULL;
	GA *ga;
	double *query_cache;

	ANT_search_engine search_engine(&memory);

	GA_stemmer *stemmer = new GA_stemmer(&search_engine);
	GA_individual *ind = new GA_individual();
	srand(time(NULL));

	//	document_list = read_docid_list(&documents_in_id_list);
	//	assessments = get_qrels(&memory, qrel_file, &number_of_assessments, qrel_format, document_list, documents_in_id_list);
	ANT_mean_average_precision *map = new ANT_mean_average_precision(&memory, assessments, number_of_assessments);

	if ((fp = fopen(topic_file, "rb")) == NULL)
		exit(fprintf(stderr, "Cannot open topic file:%s\n", topic_file));
	line = 1;
	while (fgets(query, sizeof(query), fp) != NULL) {
		strip_end_punc(query);
		topic_ids = (long *) realloc(topic_ids, line * sizeof(topic_ids[0]));
		topic_ids[line - 1] = atol(query);
		if ((query_text = strchr(query, ' ')) == NULL)
			exit(fprintf(stderr, "Line %ld: Can't process query as badly formed:'%s'\n", line, query));
		all_queries = (char **) realloc(all_queries, line * sizeof(all_queries[0]));
		all_queries[line - 1] = strdup(query_text);
		line++;
	}
	fclose(fp);

	if (stemmer_file) {
		int i;
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
			for (i = 0; i < line - 1; i++) {
				long hits;
				// TODO: Gives error about param list
				//result += perform_query_w_stemmer(&search_engine, all_queries[i], &hits, stemmer, topic_ids[i], -1, map);
			}
			fprintf(stderr, "%d %f\n", count++, result / (line - 1));
		}
	} else {
		//		ga = new GA(POPULATION_SIZE, 
		//p		new GA_function(perform_query_w_stemmer, &search_engine, line - 1, all_queries, topic_ids, query_cache, map), &search_engine);
		ga->run(NUM_OF_GENERATIONS);
	}

}

/*
				freopen(argv[4], "w", stderr);
				ga_ant(argv[2], argv[3], NULL, qrel);
				fclose(stderr);
			} else if (argc == 6) {
				freopen(argv[5], "w", stderr);
				ga_ant(argv[2], argv[3], argv[4], qrel);
				fclose(stderr);
			trie_test();
*/

#endif /* __GA_ANT_H__ */
