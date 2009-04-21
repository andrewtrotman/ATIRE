#include <string.h>
#include <stdlib.h>
#include "vocab.h"
#include "btree_iterator.h"
#include "ga_individual.h"

/*
  TODO: WEIGHT ACORDING TO FREQUENCY 
  
 */
vocab::vocab(ANT_search_engine *search_engine) {
    char *term;
    int i;
    ANT_btree_iterator iterator(search_engine);

    for (string_count = 0, iterator.first(NULL); iterator.next() != NULL; string_count++)
        ;

    strings = (char **) malloc(sizeof(strings[0]) * string_count);
    for (i = 0, term = iterator.first(NULL); term != NULL; term = iterator.next(), i++)
        strings[i] = strdup(term);
}

void vocab::weight_strings() {
}

char *vocab::strgen() {
    char *str = strings[rand() % string_count];
    int len = strlen(str);
    str += len;
    if (len > RULE_STRING_MAX)
        len = RULE_STRING_MAX;
    return str - (rand() % len + 1);
}

char *vocab::strgen_2() {
    char *str = strings[rand() % string_count];
    int len = strlen(str);
    str += len;
    if (len > RULE_STRING_MAX)
        len = RULE_STRING_MAX;
    return str - rand() % (len + 1);
}
