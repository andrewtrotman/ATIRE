/*
	STEMMER_TERM_SIMILARITY_THRESHOLD.H
	-----------------------------------
*/
#ifndef STEMMER_TERM_SIMILARITY_THRESHOLD_H_
#define STEMMER_TERM_SIMILARITY_THRESHOLD_H_

#include "stemmer.h"
#include "stemmer_term_similarity.h"

class ANT_search_engine;

/*
	class ANT_STEMMER_TERM_SIMILARITY_THRESHOLD
	-------------------------------------------
*/
class ANT_stemmer_term_similarity_threshold : public ANT_stemmer_term_similarity
{
private:
    double threshold;

public:
	ANT_stemmer_term_similarity_threshold(ANT_search_engine *search_engine, ANT_stemmer *stemmer, double threshold);
    virtual ~ANT_stemmer_term_similarity_threshold() {}

    virtual char *first(char *start);
    virtual char *next(void);
} ;

#endif  /* STEMMER_TERM_SIMILARITY_THRESHOLD_H_ */
