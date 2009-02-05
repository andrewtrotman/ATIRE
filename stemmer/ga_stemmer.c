#include <stdio.h>
#include <string.h>
#include "ga_stemmer.h"
#include "counter.h"

long GA_stemmer::stem(char *term, char *destination) {
    return strlen(strcpy(destination, individual->apply(term)));
}

char *GA_stemmer::get_next(char *from) {
    char *term_to_check;

    for (term_to_check = from; term_to_check != NULL; term_to_check = ANT_btree_iterator::next())
        {
            stem(term_to_check, stemmed_index_term);
            if (strcmp(stemmed_index_term, stemmed_search_term) == 0) {
                //		printf("%s\n", term_to_check);
                INC_COUNTER;
                return term_to_check;
            }
            if (strncmp(term_to_check, stemmed_search_term, stemmed_search_term_length) != 0)
                return NULL;
        }
    return NULL;
}

char *GA_stemmer::first(char *term) {
    stemmed_search_term_length = stem(term, stemmed_search_term);
    strncpy(stemmed_prefix, term, FIXED_SEARCH_LENGTH);
    stemmed_prefix[FIXED_SEARCH_LENGTH] = '\0';
//    return get_next(ANT_btree_iterator::first(stemmed_prefix));
    return get_next(ANT_btree_iterator::first(stemmed_search_term));
}

void GA_stemmer::print() {
    individual->print_raw();
}
