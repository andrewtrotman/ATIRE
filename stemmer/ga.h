#ifndef __GA_H__
#define __GA_H__

#include "ga_function.h"
#include "ga_individual.h"

const unsigned int DEFAULT_TOURNAMENT_SIZE = 2;
const unsigned int DEFAULT_MUTATION_RATE = 10; // out of 100
const unsigned int DEFAULT_CROSSOVER_RATE = 80; // out of 100
enum { NO_ELITISM, USE_ELITISM };

class Vocab;

class GA {
 private:
    Vocab *vocab;
    GA_function *fitness_function;

    GA_individual *population;
    GA_individual *next_population;

    unsigned int population_size;
    unsigned int generation;

    int elitism;

    unsigned int mutation_rate;
    unsigned int crossover_rate;
    unsigned int tournament_size;

    inline double get_fitness(GA_individual *);

    GA_individual *tournament_select();
    void next_generation();
    GA_individual *get_best();

 public:
    void run(unsigned int);
    void set_generations(unsigned int);

    GA(unsigned int, GA_function *, ANT_search_engine *);
    ~GA();
};

#endif  /* __GA_H__ */
