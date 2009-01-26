#include <string.h>
#include <stdlib.h>
#include "strgen.h"
#include "btree_iterator.h"

char **strings;
int string_count;

void init_strgen(ANT_search_engine *search_engine) {
    char *term;
    int i;
    ANT_btree_iterator iterator(search_engine);

    for (i = 0, iterator.first(NULL); iterator.next() != NULL; i++)
        ;

    string_count = i;
    strings = (char **) malloc(sizeof(strings[0]) * string_count);

    for (i = 0, iterator.first(NULL); term != NULL; term = iterator.next(), i++)
        strings[i] = strdup(term);
}

char *strgen() {
    char *str = strings[rand() % string_count];
    int len = strlen(str);
    return str + rand() % len;
}
