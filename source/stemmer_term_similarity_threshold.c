/*
	STEMMER_TERM_SIMILARITY_THRESHOLD.C
	-----------------------------------
*/
#include <string.h>
#include "stemmer_term_similarity.h"
#include "stemmer_term_similarity_threshold.h"

/*
	ANT_STEMMER_TERM_SIMILARITY_THRESHOLD::ANT_STEMMER_TERM_SIMILARITY_THRESHOLD()
	----------------------------------------------------------
*/
ANT_stemmer_term_similarity_threshold::ANT_stemmer_term_similarity_threshold(ANT_search_engine *search_engine, ANT_stemmer *stemmer, double threshold) : ANT_stemmer_term_similarity(search_engine, stemmer)
{
this->threshold = threshold;
}

/*
	ANT_STEMMER_TERM_SIMILARITY_THRESHOLD::NEXT()
	---------------------------------------------
*/
char *ANT_stemmer_term_similarity_threshold::next()
{
char *next = base_stemmer->next();

if (next == NULL)
	return next;
while (next != NULL &&  (strcmp(term, next) != 0) && buffer_similarity(next) < threshold)   
	next = base_stemmer->next();

return next;
}

/*
	ANT_STEMMER_TERM_SIMILARITY_THRESHOLD::FIRST()
	----------------------------------------------
*/
char *ANT_stemmer_term_similarity_threshold::first(char *term)
{
char *first = base_stemmer->first(term);

if (first == NULL)
	return NULL;

strncpy(this->term, term, MAX_TERM_LENGTH);
buffer_length_squared = fill_buffer_with_postings(term, buffer, &document_frequency);
if (strcmp(this->term, first) == 0)
	return first;

while (first != NULL && buffer_similarity(first) < threshold)
	first = base_stemmer->next();

return first;
}

