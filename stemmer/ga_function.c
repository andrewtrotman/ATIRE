#include "ga_function.h"
#include "ga_stemmer.h"

double GA_function::call() {
    int i;
    double sum_of_average_precisions = 0.0;
    long hits;
    for (i = 0; i < query_count; i++) {
        sum_of_average_precisions += function(search_engine, all_queries[i], &hits, stemmer, topic_ids[i], cached_results[i], map);
    }
    if (sum_of_average_precisions == 0.0)
        stemmer->print();
    return sum_of_average_precisions / (double) (query_count);
}

GA_function::GA_function(double (*function) (ANT_search_engine *, char *, long *, ANT_stemmer *, long, double, ANT_mean_average_precision *),
                         ANT_search_engine *search_engine, long query_count, char **all_queries, long *topic_ids, 
                         double *cached_results, ANT_mean_average_precision *map) {
    this->function = function;
    this->search_engine = search_engine;
    this->query_count = query_count;
    this->all_queries = all_queries;
    this->topic_ids = topic_ids;
    this->map = map;
    this->cached_results = cached_results;
    stemmer = new GA_stemmer(search_engine);
}
