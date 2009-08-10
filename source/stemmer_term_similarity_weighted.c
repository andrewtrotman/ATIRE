/*
	STEMMER_TERM_SIMILARITY_WEIGHTED.C
	----------------------------------
*/
#include <string.h>
#include <math.h>
#include "stemmer.h"
#include "stemmer_term_similarity.h"
#include "stemmer_term_similarity_weighted.h"
#include "ranking_function.h"

/*
	ANT_STEMMER_TERM_SIMILARITY_WEIGHTED::ANT_STEMMER_TERM_SIMILARITY_WEIGHTED()
	----------------------------------------------------------
*/
ANT_stemmer_term_similarity_weighted::ANT_stemmer_term_similarity_weighted(ANT_search_engine *search_engine, ANT_stemmer *stemmer, double weight) : ANT_stemmer_term_similarity(search_engine, stemmer)
{
this->weight = weight;
}

/*
	ANT_STEMMER_TERM_SIMILARITY_WEIGHTED::WEIGHT_TERMS()
	----------------------------------------------------
        Ensure that the term weight for the term itself is always 1.0, otherwise you will get weirdness.
        Also, weights probably shouldn't exceed 1.0.
*/
long ANT_stemmer_term_similarity_weighted::weight_terms(ANT_weighted_tf *term_weight, char *term)
{
if (strcmp(this->term, term) == 0)
    *term_weight = (ANT_weighted_tf) 1.0;
else
    *term_weight = (ANT_weighted_tf) pow(buffer_similarity(term), weight);
return 1;
}
