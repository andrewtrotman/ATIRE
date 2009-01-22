#ifndef __GA_FUNCTION__
#define __GA_FUNCTION__

#include "search_engine.h"
#include "ga_individual.h"

class GA_function {
private:
    ANT_search_engine *search_engine;
    int query_count;
    char **all_queries;
    long *topic_ids;
    ANT_mean_average_precision *map;
    double (*function) (ANT_search_engine *,
                        char *, long *, long, ANT_mean_average_precision *);
public:
    GA_individual *stemmer;
    double call();
    GA_function(double (*)(ANT_search_engine *, char *, long *, long, ANT_mean_average_precision *),
            ANT_search_engine *,
            long, char **, long *, ANT_mean_average_precision *);
    ~GA_function() { free(all_queries); };
};

#endif
