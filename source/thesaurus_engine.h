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

class ANT_stemmer;

/*
	class ANT_THESAURUS_ENGINE
	--------------------------
*/
class ANT_thesaurus_engine : public ANT_search_engine
{
private:
	long *buffer_a;
	long *buffer_b;
	double threshold;

private:
	long long fill_buffer_with_postings(char *term, long *buffer);
	double buffer_similarity(long long buffer_a_total, long long buffer_b_total);
    double prob_word_in_query(char **query_terms, int query_term_count, char *word, double p_coll_w, long long documents_returned, double *p_q_d_buffer, double lambda); 

public:
	ANT_thesaurus_engine(ANT_memory *memory, double threshold) : ANT_search_engine(memory)
		{
		buffer_a = (long *)memory->malloc(sizeof (*buffer_a) * documents);
		buffer_b = (long *)memory->malloc(sizeof (*buffer_b) * documents);
		this->threshold = threshold;
		}

	double term_similarity(char *term1, char *term2);
	void stemming_exceptions(ANT_stemmer *stemmer, double threshold);
	virtual void process_one_stemmed_search_term(ANT_stemmer *stemmer, char *base_term, ANT_ranking_function *ranking_function);
    double clarity_score(char *query, double lambda = 0.6);
};


#endif __THESAURUS_ENGINE_H__
