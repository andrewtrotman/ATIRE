/*
	STEMMER_TERM_SIMILARITY_WEIGHTED.H
	----------------------------------
*/
#ifndef STEMMER_TERM_SIMILARITY_WEIGHTED_H_
#define STEMMER_TERM_SIMILARITY_WEIGHTED_H_

#include "stemmer.h"
#include "stemmer_term_similarity.h"
#include "ranking_function.h"

class ANT_search_engine;

/*
	class ANT_STEMMER_TERM_SIMILARITY_WEIGHTED
	------------------------------------------
*/
class ANT_stemmer_term_similarity_weighted : public ANT_stemmer_term_similarity
{
private:
	double weight;

public:
	ANT_stemmer_term_similarity_weighted(ANT_search_engine *search_engine, ANT_stemmer *stemmer, double weight);
	virtual ~ANT_stemmer_term_similarity_weighted() {}

    virtual long weight_terms(ANT_weighted_tf *term_weight, char *term);
} ;

#endif  /* STEMMER_TERM_SIMILARITY_WEIGHTED_H_ */
