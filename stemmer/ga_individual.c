#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "strnlen.h"
#include "ga_individual.h"

/* 
   Remember that strings in rules may not be null terminated!
   (But they do have a maximum length, so use "%.*s", 6, string)
*/

inline static int consonant_p(const char *s, int i) {
    switch (s[i]) {
    case 'a': case 'e': case 'i': case 'o': case 'u': return FALSE;
    case 'y': return (i == 0) ? TRUE : !consonant_p(s, i-1);
    default: return TRUE;
    }
}

/*
  Returns the measure of a string, 
  j being the start of the suffix to remove 
*/
inline static int m(const char *s, int j) {
    int n = 0;
    int i = 0;
    while(TRUE) {
        if (i > j) return n;
        if (!consonant_p(s, i)) break; 
        i++;
    }
    i++;
    while(TRUE) {
        while(TRUE) { 
            if (i > j) return n;
            if (consonant_p(s, i)) break;
            i++;
        }
        i++;
        n++;
        while(TRUE) {
            if (i > j) return n;
            if (!consonant_p(s, i)) break;
            i++;
        }
        i++;
    }
}

inline unsigned int GA_individual::rules_size() {
    return count * RULE_SIZE;
}

inline char GA_individual::measure(unsigned int n) {
    return rules[n * RULE_SIZE];
}

inline char *GA_individual::rule_from(unsigned int n) {
    return rules + (n * RULE_SIZE) + 1;
}

inline char *GA_individual::rule_to(unsigned int n) {
    return rules + (n * RULE_SIZE) + RULE_STRING_MAX + 1;
}


/*
  Currently returns a pointer to a static array - do not free!
*/
char *GA_individual::apply(const char *string) {
    unsigned int i, skipping = 0;
    int length;
    static char buffer[TMP_BUFFER_SIZE];
    
    strncpy(buffer, string, TMP_BUFFER_SIZE);
    buffer[TMP_BUFFER_SIZE - 1] = '\0';
    length = strlen(buffer);

    for (i = 0; i < count; i++) {
        if (measure(i) == SEPARATOR) {
            /* Stop skipping at separator */
            skipping = 0;
        } else if (!skipping) {
            /* Check that rule can be applied */

            if (m(buffer, length - 1) >= measure(i) &&
                strncmp(buffer + length - strnlen(rule_from(i), 
                                                  RULE_STRING_MAX),
                        rule_from(i),
                        RULE_STRING_MAX) == 0) {
                strncpy(buffer + length - strnlen(rule_from(i), 
                                                  RULE_STRING_MAX),
                        rule_to(i),
                        RULE_STRING_MAX);

                buffer[length 
                       - strnlen(rule_from(i), RULE_STRING_MAX) 
                       + strnlen(rule_to(i), RULE_STRING_MAX)] = '\0';
                length -= strnlen(rule_from(i), RULE_STRING_MAX);
                length += strnlen(rule_to(i), RULE_STRING_MAX);

                /*
                  TODO: 
                  only use strnlen once.
                */

                skipping = 1;
            }
        }
    }

    return buffer;
}

void GA_individual::print() {
    unsigned int i;
    for (i = 0; i < count; i++) {
        if (this->measure(i) == SEPARATOR) {
            printf("=SEP=\n");
        } else {
            printf("if (M >= %d) substitute '%.*s' for '%.*s'\n",
                   this->measure(i), 
                   RULE_STRING_MAX, this->rule_from(i),
                   RULE_STRING_MAX, this->rule_to(i));
        }
    }
}

void GA_individual::print_raw() {
    unsigned int i;
    for (i = 0; i < this->rules_size(); i++) {
        if (rules[i] >= 'a' && rules[i] <= 'z')
            putchar(rules[i]);
        else 
            printf("[%d]", (int) rules[i]);
    }
    putchar('\n');
}

/*
  The three following operations: REPRODUCE() MUTATE() and CROSSOVER()
  assume all space has been allocated and the base size is the same.
  (i.e. char[FIXEDSIZE] within the class)
 */
void GA_individual::reproduce(GA_individual *c) {
    memcpy(c, this, sizeof(GA_individual));
}

void GA_individual::mutate(GA_individual *c, char *(*str_gen)()) {
    c->generate(str_gen);
    if (rand() % 2 == 0) 
        this->crossover(c,c);
    else 
        c->crossover(this,c);
}

/* 
   Places a new individual into c based on the parents p1 & p2.

   TODO: ensure that the INDIVIDUAL_SIZE_MAX limit is never exceeded
   do this when the range for the second point is being decided.
*/
void GA_individual::crossover(GA_individual *p2, GA_individual *c) {
    unsigned int point = random_from(0, this->rules_size());
    /* Ensure the second point shares the mid-rule position of the first */
    unsigned int point2 = random_from(0, p2->count) * RULE_SIZE
        + (point % RULE_SIZE);
    if (this != c)
        memcpy(c->rules, this->rules, point);
    if (p2 != c)
        memcpy(c->rules + point, p2->rules + point2, p2->rules_size() - point2);
    c->is_evaluated = FALSE;
    c->count = point + (p2->rules_size() - point2);
}

/* 
   Generates new individuals using strings from a string gen function.
   No allocation or deallocation (or modification) is done on the strings
   ONLY POINTERS ARE NEEDED.

   Strings will be used until null-termed or over 6 chars
*/
void GA_individual::generate(char *(*str_gen)()) {
    unsigned int i;
    count = (unsigned int) rand() % (MAX_RULES) + 1;
    memset(rules, '\0', count * RULE_SIZE);

    for (i = 0; i < count; i++) {
        rules[i * RULE_SIZE] = (char) (rand() % (MEASURE_MAX + 1));
        if (rules[i * RULE_SIZE] == MEASURE_MAX)
            rules[i * RULE_SIZE] = SEPARATOR;

        strncpy(rules + (i * RULE_SIZE) + 1,
                str_gen(),
                RULE_STRING_MAX);
        strncpy(rules + (i * RULE_SIZE) + RULE_STRING_MAX + 1,
                str_gen(),
                RULE_STRING_MAX);
    }
}
