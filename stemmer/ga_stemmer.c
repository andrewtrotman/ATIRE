#include <stdio.h>
#include <string.h>
#include "ga_stemmer.h"

size_t GA_stemmer::stem(char *term, char *destination) {
    return strlen(strcpy(destination, individual->apply(term)));
}

char *GA_stemmer::get_next(char *from) {
    char *term_to_check;

    for (term_to_check = from; term_to_check != NULL; term_to_check = ANT_btree_iterator::next())
        {
            stem(term_to_check, stemmed_index_term);
            if (strcmp(stemmed_index_term, stemmed_search_term) == 0) {
                //		printf("%s\n", term_to_check);
#ifdef GA_STEMMER_STATS
				individual->keep_last_stats();
#endif 
                return term_to_check;
            }
            if (strncmp(term_to_check, stemmed_search_term, FIXED_SEARCH_LENGTH) != 0)
                return NULL;
        }
    return NULL;
}

char *GA_stemmer::first(char *term) {
    stemmed_search_term_length = stem(term, stemmed_search_term);
    strncpy(stemmed_prefix, term, FIXED_SEARCH_LENGTH);
    stemmed_prefix[FIXED_SEARCH_LENGTH] = '\0';

    return get_next(ANT_btree_iterator::first(stemmed_prefix));
}

void GA_stemmer::print(FILE *fd) {
    individual->print_raw(fd);
#ifdef GA_STEMMER_STATS
	individual->print_rule_usage(fd);
#endif 
}
