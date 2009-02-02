#include <stdio.h>
#include <string.h>
#include "ga_stemmer.h"

long GA_stemmer::stem(char *term, char *destination) {
    return strlen(strcpy(destination, individual->apply(term)));
}

char *GA_stemmer::get_next(char *from)
{
char *term_to_check;

for (term_to_check = from; term_to_check != NULL; term_to_check = ANT_btree_iterator::next())
	{
	stem(term_to_check, stemmed_index_term);
	if (strcmp(stemmed_index_term, stemmed_search_term) == 0) {
//		printf("%s\n", term_to_check);
        return term_to_check;
    }
	if (strncmp(term_to_check, stemmed_search_term, stemmed_search_term_length) != 0)
		return NULL;
	}
return NULL;
}

char *GA_stemmer::first(char *term) {
    stemmed_search_term_length = FIXED_SEARCH_LENGTH;
    stem(term, stemmed_search_term);
    return get_next(ANT_btree_iterator::first(stemmed_search_term));
}
