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
    double prob_word_in_query(char *query_term, ANT_search_engine_accumulator::ANT_accumulator_t *full_query_results, ANT_ranking_function *ranking_function);

public:
	ANT_thesaurus_engine(ANT_memory *memory) : ANT_search_engine(memory) {}
    double clarity_score(char *query, ANT_ranking_function *ranking_function);
};


#endif __THESAURUS_ENGINE_H__
