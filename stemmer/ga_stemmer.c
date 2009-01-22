#include <string.h>
#include "ga_stemmer.h"

long GA_stemmer::stem(char *term, char *destination) {
    return strlen(strcpy(destination, individual->apply(term)));
}
