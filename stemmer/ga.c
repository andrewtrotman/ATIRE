#include <stdlib.h>
#include "ga.h"
#include "ga_individual.h"

/* note that the pointer returned is not to a newly made object */
GA_individual *GA::tournament_select() {
    GA_individual *pick = population + (rand() % population_size);
    GA_individual *tmp;
    unsigned int i;

    for(i = 0; i < tournament_size - 1; i++) {
        tmp = population + (rand() % population_size);

        if (tmp->fitness > pick->fitness)
            pick = tmp;
    }

    return pick;
}

void GA::next_generation() {
    unsigned int i, op;

    for (i = 0; i < population_size; i++) {
        op = random_from(0, 100);
        if (op < mutation_rate)
            GA_individual::mutate(tournament_select(), next_population + i);
        else if (op < mutation_rate + crossover_rate) 
            GA_individual::crossover(tournament_select(), tournament_select(),
                                     next_population + i);
        else 
            GA_individual::reproduce(tournament_select(), next_population + i);
    }

    {
        GA_individual *tmp = population;
        population = next_population;
        next_population = tmp;
    }
}

GA_individual *GA::get_best() {
    unsigned int i;
    GA_individual *best = population;

    for (i = 1; i < population_size; i++) {
        if (population[i].fitness > best->fitness)
            best = population + i;
    }
    return best;
}

void GA::run(unsigned int generations) {
    unsigned int i;
    for (i = 0; i < generations; i++) {
        this->next_generation();
    }
}

GA::GA(unsigned int population_size, GA_func *fitness_function) {
    this->population_size = population_size;
    this->fitness_function = fitness_function;

    population = (GA_individual *) 
        malloc(sizeof population[0] * population_size);
    next_population = (GA_individual *) 
        malloc(sizeof next_population[0] * population_size);

    elitism = NO_ELITISM;

    mutation_rate = DEFAULT_MUTATION_RATE;
    crossover_rate = DEFAULT_CROSSOVER_RATE;
    tournament_size = DEFAULT_TOURNAMENT_SIZE;
}

GA::~GA() {
    free(population);
}

double GA_func::call() {
    int i;
    double sum_of_average_precisions = 0.0;
    long hits;
    for (i = 0; i < query_count; i++) {
        sum_of_average_precisions += function(search_engine, all_queries[i], &hits, topic_ids[i], map);
    }
    return sum_of_average_precisions / (double) (query_count);
}

GA_func::GA_func(double (*function) (ANT_search_engine *, char *, long *, long, ANT_mean_average_precision *),
                 ANT_search_engine *search_engine, long query_count, char **all_queries, long *topic_ids, 
                 ANT_mean_average_precision *map) {
    this->function = function;
    this->search_engine = search_engine;
    this->query_count = query_count;
    this->all_queries = all_queries;
    this->topic_ids = topic_ids;
    this->map = map;
}
