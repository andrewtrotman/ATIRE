#ifndef __GA_H__
#define __GA_H__

#include "search_engine.h"
#include "ga_individual.h"

const unsigned int DEFAULT_TOURNAMENT_SIZE = 2;
const unsigned int DEFAULT_MUTATION_RATE = 10; // out of 100
const unsigned int DEFAULT_CROSSOVER_RATE = 70; // out of 100
enum { NO_ELITISM, USE_ELITISM };

class GA_func {
private:
    ANT_search_engine *search_engine;
    int query_count;
    char **all_queries;
    long *topic_ids;
    ANT_mean_average_precision *map;
    double (*function) (ANT_search_engine *,
                        char *, long *, long, ANT_mean_average_precision *);
public:
    double call();
    GA_func(double (*)(ANT_search_engine *, char *, long *, long, ANT_mean_average_precision *),
            ANT_search_engine *,
            long, char **, long *, ANT_mean_average_precision *);
    ~GA_func() { free(all_queries); };
};

class GA {
 private:
    GA_func *fitness_function;

    GA_individual *population;
    GA_individual *next_population;

    unsigned int population_size;
    unsigned int generation;

    int elitism;

    unsigned int mutation_rate;
    unsigned int crossover_rate;
    unsigned int tournament_size;

    GA_individual *tournament_select();
    void next_generation();
    GA_individual *get_best();
 public:
    void run(unsigned int);
    void set_generations(unsigned int);

    GA(unsigned int, GA_func *);
    ~GA();
};

#endif  /* __GA_H__ */
