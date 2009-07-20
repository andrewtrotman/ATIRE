#ifndef __GA_FUNCTION__
#define __GA_FUNCTION__

#include "search_engine.h"
#include "stemmer.h"
#include "ga_stemmer.h"
#include "mean_average_precision.h"
#include "ant_param_block.h"
#include "ranking_function.h"

typedef double (*ga_func_t)(ANT_ANT_param_block *,ANT_search_engine *, ANT_ranking_function *, char *, long long *, 
                   long, ANT_mean_average_precision *, ANT_stemmer *);

class GA_function {
private:
	ANT_ANT_param_block *param_block;
    ANT_search_engine *search_engine;
    ANT_ranking_function *ranking_function;
    int query_count;
    char **queries;
    long *topic_ids;
    double *cached_results;
    ANT_mean_average_precision *map;
    ga_func_t function;

public:
    GA_stemmer *stemmer;
    double call();
    GA_function(ga_func_t, ANT_ANT_param_block *, ANT_search_engine *, ANT_ranking_function *, long, char **, long *,
				ANT_mean_average_precision *, GA_stemmer *);
    ~GA_function() { free(queries); };
};

#endif
