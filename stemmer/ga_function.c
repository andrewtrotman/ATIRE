#include "ga_function.h"
#include "ga_stemmer.h"

double GA_function::call() {
    int i;
    double sum_of_average_precisions = 0.0;
    long long hits;
    for (i = 0; i < query_count; i++) {
        sum_of_average_precisions += function(param_block, search_engine, ranking_function, queries[i], &hits, 
											  topic_ids[i], map, stemmer);
    }
    return sum_of_average_precisions / (double) (query_count);
}

GA_function::GA_function(ga_func_t function, ANT_ANT_param_block *param_block, ANT_search_engine *search_engine,
                         ANT_ranking_function *ranking_function,
						 long query_count, char **queries, long *topic_ids, ANT_mean_average_precision *map,
                         GA_stemmer *stemmer) {
	this->param_block = param_block;
    this->function = function;
    this->search_engine = search_engine;
    this->ranking_function = ranking_function;
    this->query_count = query_count;
    this->queries = queries;
    this->topic_ids = topic_ids;
    this->map = map;
    this->stemmer = stemmer; 
}
