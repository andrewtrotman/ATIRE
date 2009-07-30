/*
	THESAURUS_ENGINE.H
	------------------
*/
#ifndef __THESAURUS_ENGINE_H__
#define __THESAURUS_ENGINE_H__

/* 
   A bus crashed into a truck full of thesauri.
   Onlookers were stunned, amazed, appalled, astounded, shocked, surprised, awed...  
*/

#include "search_engine.h"
#include "memory.h"
#include "search_engine_accumulator.h"

class ANT_stemmer;

/*
	class ANT_THESAURUS_ENGINE
	--------------------------
*/
class ANT_thesaurus_engine : public ANT_search_engine
{
private:
    char *buffer_a_term;
	long *buffer_a;
    long long buffer_a_length_squared;
	double threshold;

private:
	long long fill_buffer_with_postings(char *term, long *buffer);
	double buffer_similarity(char *a, char *b);
    double prob_word_in_query(char *query_term, ANT_search_engine_accumulator::ANT_accumulator_t *full_query_results, ANT_ranking_function *ranking_function);

public:
	ANT_thesaurus_engine(ANT_memory *memory, double threshold) : ANT_search_engine(memory)
		{
        buffer_a_term = NULL;
		buffer_a = (long *)memory->malloc(sizeof (*buffer_a) * documents);
		this->threshold = threshold;
		}

	double term_similarity(char *term1, char *term2);
	void stemming_exceptions(ANT_stemmer *stemmer, double threshold);
	virtual void process_one_stemmed_search_term(ANT_stemmer *stemmer, char *base_term, ANT_ranking_function *ranking_function);
    double clarity_score(char *query, ANT_ranking_function *ranking_function);
};


#endif __THESAURUS_ENGINE_H__
