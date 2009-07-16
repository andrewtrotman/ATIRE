#ifndef _THESAURUS_ENGINE_
#define _THESAURUS_ENGINE_

/* 
   A bus crashed into a truck full of thesauri.
   Onlookers were stunned, amazed, appalled, astounded, shocked, surprised, awed...  
*/

#include "search_engine.h"
#include "memory.h"

class ANT_stemmer;

class thesaurus_engine : public ANT_search_engine {
private:
    long long fill_buffer_with_postings(char *term, long *buffer);
    double buffer_similarity(long long buffer_a_total, long long buffer_b_total);
    long *buffer_a;
    long *buffer_b;
    double threshold;

public:
    thesaurus_engine(ANT_memory *memory, double threshold) : ANT_search_engine(memory) {
        buffer_a = (long *)memory->malloc(sizeof (*buffer_a) * documents);
        buffer_b = (long *)memory->malloc(sizeof (*buffer_b) * documents);
        this->threshold = threshold;
    }
    double term_similarity(char *term1, char *term2);
    void stemming_exceptions(ANT_stemmer *, double);
    void process_one_stemmed_search_term(ANT_stemmer *stemmer, char *base_term);
};

#endif
