#ifndef __GA_INDIVIDUAL_H__
#define __GA_INDIVIDUAL_H__

#include <stdio.h>
#include <stdlib.h>

const int TMP_BUFFER_SIZE = 255;
const int RULE_STRING_MAX = 4;
const int RULE_SIZE = (RULE_STRING_MAX * 2 + 1);
const int SEPARATOR = -1;
const int MAX_RULES = 60;
const int GEN_MIN_RULES = 5;    // Minimum size when generating.
const int GEN_MAX_RULES = 30;   // Maximum size when generating.
const int MAX_INDIVIDUAL_SIZE = (RULE_SIZE * MAX_RULES);
const int MEASURE_MAX = 5;
const int SACROSANCT_CHARS = 3;

class Vocab;

/* And now some philosophy: */
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif 

/* Generates a random number within a given range. */
extern inline unsigned int random_from(unsigned int lower, unsigned int upper) {
    if (lower >= upper) return lower;
    return (rand() % (upper - lower)) + lower; 
}

class GA_individual {
 private:
	long current_usage_stats[MAX_RULES];
	long rule_usage_stats[MAX_RULES];
	long rules_used_together[MAX_RULES];

    char rules[MAX_INDIVIDUAL_SIZE];
    unsigned int count;         // Number of rules.

    inline unsigned int rules_size();
    inline char measure(unsigned int n);
    inline char *rule_from(unsigned int n);
    inline char *rule_to(unsigned int n);
    inline int is_banned(char *s);
    
    int sanity_check(); // Checks that an individual is correctly formed.

 public:

    int is_evaluated;
    double fitness;

    char *apply(const char *);
    double evaluate();
    void print();
    void print_raw(FILE *);
    void reproduce(GA_individual *);
    void mutate(GA_individual *, Vocab *);
    void crossover(GA_individual *, GA_individual *);
    void generate(Vocab *);
    void load(char *);
    void sload(char *);
    void generate_c(const char *);

	void keep_last_stats(); // For ensuring stats are those which match.
	void print_rule_usage(FILE *);
	void clear_rule_usage();
};

#endif /* __GA_INDIVIDUAL_H__ */
