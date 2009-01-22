#ifndef __GA_INDIVIDUAL_H__
#define __GA_INDIVIDUAL_H__

const int TMP_BUFFER_SIZE = 255;
const int RULE_STRING_MAX = 6;
const int RULE_SIZE = (RULE_STRING_MAX * 2 + 1);
const int SEPARATOR = -1;
const int MAX_RULES = 20;
const int MAX_INDIVIDUAL_SIZE = (RULE_SIZE * MAX_RULES);
const int MEASURE_MAX = 5;

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif 

#include <stdlib.h>
/* Generates a random number within a given range. */
extern inline unsigned int random_from(unsigned int lower, unsigned int upper) {
    if (lower >= upper) return lower;
    return (rand() % (upper - lower)) + lower; 
}

class GA_individual {
 private:
    char rules[MAX_INDIVIDUAL_SIZE];
    unsigned int count;

    inline unsigned int rules_size();
    inline char measure(unsigned int n);
    inline char *rule_from(unsigned int n);
    inline char *rule_to(unsigned int n);

 public:
    int is_evaluated;
    double fitness;

    char *apply(const char *);
    double evaluate();
    void print();
    void print_raw();
    void reproduce(GA_individual *);
    void mutate(GA_individual *, char *(*)());
    void crossover(GA_individual *, GA_individual *);
    void generate(char *(*)());
};

#endif /* __GA_INDIVIDUAL_H__ */
