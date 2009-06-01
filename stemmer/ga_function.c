#include "ga_function.h"
#include "ga_stemmer.h"

double GA_function::call() {
    int i;
    double sum_of_average_precisions = 0.0;
    long long hits;
    for (i = 0; i < query_count; i++) {
        sum_of_average_precisions += function(param_block, search_engine, all_queries[i], &hits, 
											  topic_ids[i], map, stemmer);
    }

    return sum_of_average_precisions / (double) (query_count);
}

GA_function::GA_function(double (*function)(ANT_ANT_param_block *,ANT_search_engine *, char *,
											long long *, long, ANT_mean_average_precision *,
											ANT_stemmer *),
						 ANT_ANT_param_block *param_block, ANT_search_engine *search_engine,
						 long query_count, char **all_queries, long *topic_ids,
						 ANT_mean_average_precision *map) {
	this->param_block = param_block;
    this->function = function;
    this->search_engine = search_engine;
    this->query_count = query_count;
    this->all_queries = all_queries;
    this->topic_ids = topic_ids;
    this->map = map;
    stemmer = new GA_stemmer(search_engine);
}
