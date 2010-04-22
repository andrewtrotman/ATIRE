#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "strnlen.h"
#include "ga_individual.h"
#include "vocab.h"

/*
  COMPILE OPTIONS:
    NO_SEPARATORS - don't use separators for rules
    NO_MEASURE    - don't use measure for rules
*/


//const char *banned[] = {"s"};
const char *banned[] = {};

inline int strnmatchlen(const char *a, const char *b, int n) {
    int count = 0;
    while (*a == *b && *a != '\0' && *b != '\0' && n > 0) {
        count++; n--; a++; b++;
    }
    return count;
}
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
    // 
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

/* is a word on the banned list? */
inline int GA_individual::is_banned(char *s) {
    unsigned int i;
    for (i = 0; i < (sizeof banned / sizeof banned[0]); i++) {
        if (strcmp(s, banned[i]) == 0)
            return TRUE;
    }
    return FALSE;
}

/*
  Currently returns a pointer to a static array - do not free!
*/
char *GA_individual::apply(const char *string) {
    unsigned int i, skipping = FALSE;
    int length;
#ifndef NO_MEASURE
    int buffer_m;
#endif
    static char buffer[TMP_BUFFER_SIZE];

	#ifdef GA_STEMMER_STATS
	memset(current_usage_stats, 0, sizeof rule_usage_stats);
	#endif

    strncpy(buffer, string, TMP_BUFFER_SIZE);
    buffer[TMP_BUFFER_SIZE - 1] = '\0';
    length = strlen(buffer);
#ifndef NO_MEASURE
    buffer_m = m(buffer, length - 1); // Cache measure
#endif

    for (i = 0; i < count; i++) {
#ifdef NO_SEPARATORS
        if (skipping) return buffer;
#endif
        if (measure(i) == SEPARATOR) {
            /* Stop skipping at separator */
            skipping = FALSE;
        } else if (!skipping) {
            /* Check that rule can be applied */
            int from_len = strnlen(rule_from(i), RULE_STRING_MAX); 

#ifdef NO_MEASURE
            if (strncmp(buffer + length - from_len,
                        rule_from(i), RULE_STRING_MAX) == 0) {
#else
            if (buffer_m >= measure(i) &&
                strncmp(buffer + length - from_len,
                        rule_from(i), RULE_STRING_MAX) == 0) {
#endif
                int to_len = strnlen(rule_to(i), RULE_STRING_MAX); 

                /* Ensure that the first SACROSANCT_CHARS are respected */ 
                if (length - from_len + 
                    strnmatchlen(rule_from(i), rule_to(i), RULE_STRING_MAX) < SACROSANCT_CHARS)
                    continue;

                strncpy(buffer + length - from_len, rule_to(i), RULE_STRING_MAX);

                buffer[length - from_len + to_len] = '\0';
                length += to_len - from_len;

#ifndef NO_MEASURE
                buffer_m = m(buffer, length - 1); // recalc measure
#endif                

                skipping = TRUE;

				#ifdef GA_STEMMER_STATS
				current_usage_stats[i]++;
				#endif
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

void GA_individual::print_raw(FILE *fd) {
unsigned int i;
for (i = 0; i < this->rules_size(); i++) {
    if (i % RULE_SIZE == 0) fputc(' ', fd);
    if (rules[i] >= 'a' && rules[i] <= 'z')
        fputc(rules[i], fd);
    else if (rules[i] >= 0 && rules[i] <= 9)
        fputc(rules[i] + '0', fd);
    else
        fputc('-', fd);
}
fputc('\n', fd);
}

/*
The three following operations: REPRODUCE() MUTATE() and CROSSOVER()
assume all space has been allocated and the base size is the same.
(i.e. char[FIXEDSIZE] within the class)
*/
void GA_individual::reproduce(GA_individual *c) {
    memcpy(c, this, sizeof(GA_individual));
}

/* c is the child, this is the parent */
void GA_individual::mutate(GA_individual *c, Vocab *v) {
    char *s; (void) s;
    unsigned int position = rand() % count;

    memcpy(c, this, sizeof(GA_individual));
    c->is_evaluated = FALSE;

    switch (rand() % 3) {
    case 0:
        c->rules[position * RULE_SIZE] = (char) (rand() % (MEASURE_MAX + 1));
        if (c->rules[position * RULE_SIZE] == MEASURE_MAX)
            c->rules[position * RULE_SIZE] = SEPARATOR;
        break;

    case 1:
        char *s;
        while ((s = v->strgen()) && is_banned(s)) {}
        strncpy(c->rules + (position * RULE_SIZE) + 1, s, RULE_STRING_MAX);
        break;

    case 2:
        while ((s = v->strgen_2()) && is_banned(s)) {}
        strncpy(c->rules + (position * RULE_SIZE) + RULE_STRING_MAX + 1, s, RULE_STRING_MAX);
        break;

    default:
        fprintf(stderr, "Error in GA_individual::mutate\n");
    }

#ifdef GA_STEMMER_STATS
	memset(c->rule_usage_stats, 0, sizeof c->rule_usage_stats);
#endif
}

/* 
   Places a new individual into c based on the parents p1 & p2.
   Crossover is limited to being between parts of rules (i.e. not mid-string)
*/
void GA_individual::crossover(GA_individual *p2, GA_individual *c) {
    /* which rule are we crossing over at*/
    unsigned int point = random_from(0, this->count);

    /* Prevent growth beyond the MAX_RULES */
    unsigned int min_point2 = ((signed)p2->count + (signed)point - MAX_RULES > 0) ? 
        (p2->count + point - MAX_RULES) : 0;

    unsigned int point2 = random_from(min_point2, p2->count);

    switch (random_from(0, 2)) {
    case 0: // MEASURE
        point = point * RULE_SIZE;
        point2 = point2 * RULE_SIZE;
        break;
    case 1: // RULE_FROM
        point = rule_from(point) - rules;
        point2 = p2->rule_from(point2) - p2->rules;
        break;
    case 2: // RULE_TO
        point = rule_to(point) - rules;
        point2 = p2->rule_to(point2) - p2->rules;
        break;
    default:
        fprintf(stderr, "ERROR IN CROSSOVER\n");
    }

    /* Still need to copy if p2 is also c, as the part from p1 may be small (i.e. p2 needs to shift)
     * This is also why it is done first. */
    memmove(c->rules + point, p2->rules + point2, p2->rules_size() - point2);

    if (this != c)
        memcpy(c->rules, this->rules, point);

    c->is_evaluated = FALSE;
    c->count = (point + (p2->rules_size() - point2)) / RULE_SIZE;

#ifdef GA_DEBUG
    if (c->sanity_check() == 0) {
        printf("Crossover error, point:%u\n min_point2:%u point2:%u\n", point, min_point2, point2);
        print_raw(stdout);
        p2->print_raw(stdout);
        c->print_raw(stdout);
    }
#endif
#ifdef GA_STEMMER_STATS
	memset(c->rule_usage_stats, 0, sizeof c->rule_usage_stats);
#endif
}

/* 
   Generates new individuals using strings from a string gen function.
   No allocation or deallocation (or modification) is done on the strings
   ONLY POINTERS ARE NEEDED.

   Strings will be used until null-termed or over they exceed RULE_STRING_MAX chars
*/
void GA_individual::generate(Vocab *v) {
    unsigned int i;
    
    is_evaluated = FALSE;
    count = (unsigned int) rand() % (GEN_MAX_RULES - GEN_MIN_RULES) + GEN_MIN_RULES;
    memset(rules, '\0', count * RULE_SIZE);

    for (i = 0; i < count; i++) {
        rules[i * RULE_SIZE] = (char) (rand() % (MEASURE_MAX + 1));
        if (rules[i * RULE_SIZE] == MEASURE_MAX)
            rules[i * RULE_SIZE] = SEPARATOR;

        /* 
           Checking both strings in the representation against the banned
           list ensures that there are no rules that perform the stem by 
           adding an ending. 
        */
        char *s;
        while ((s = v->strgen()) && is_banned(s)) {}
        strncpy(rules + (i * RULE_SIZE) + 1, s, RULE_STRING_MAX);

        while ((s = v->strgen_2()) && is_banned(s)) {}
        strncpy(rules + (i * RULE_SIZE) + RULE_STRING_MAX + 1, s, RULE_STRING_MAX);
    }
}

void GA_individual::load(char *filename) {
    char buffer[4096];
    int current = 0;
    char *ptr;
    FILE *handle = fopen(filename, "r");
    while (fscanf(handle, "%s", buffer) != EOF) {
        ptr = buffer;
        while (*ptr) {
            if (ptr[0] >= '0' && ptr[0] <= '9') 
                rules[current] = ptr[0] - '0';
            else if (ptr[0] == '-')
                rules[current] = (char) -1;
            else
                rules[current] = ptr[0];
            ptr++;
            current++;
        }
    }
    fclose(handle);
    count = current / RULE_SIZE;
}

void GA_individual::sload(char *string) {
    int current = 0;
    char *ptr = string;
    while (*ptr) {
        if (ptr[0] >= '0' && ptr[0] <= '9') 
            rules[current] = ptr[0] - '0';
        else if (ptr[0] == '-')
            rules[current] = (char) -1;
        else if (ptr[0] == ' ' || ptr[0] == '\n')
            current--;
        else
            rules[current] = ptr[0];
        ptr++;
        current++;
    }
    count = current / RULE_SIZE;
}

void GA_individual::generate_c(const char *filename) {
    FILE *file = fopen(filename, "w");
    unsigned int i;
    fprintf(file, "\n\
#include <string.h>\n\
\n\
#define TMP_BUFFER_SIZE %d\n\
#define SACROSANCT_CHARS %d\n\
\n\
#ifndef FALSE\n\
	#define FALSE 0\n\
#endif\n\
#ifndef TRUE\n\
	#define TRUE (!FALSE)\n\
#endif\n\
\n\
inline static int consonant_p(const char *s, int i) {\n\
    switch (s[i]) {\n\
    case 'a': case 'e': case 'i': case 'o': case 'u': return FALSE;\n\
    case 'y': return (i == 0) ? TRUE : !consonant_p(s, i-1);\n\
    default: return TRUE;\n\
    }\n\
}\n\
inline static int m(const char *s, int j) {\n\
    int n = 0, i = 0;\n\
    while(1) {\n\
    	if (i > j)\n\
    		return n;\n\
    	if (!consonant_p(s, i))\n\
	    	break;\n\
    	i++;\n\
	}\n\
    i++;\n\
    while(1) {\n\
	    while(1) {\n\
    		if (i > j)\n\
    			return n;\n\
    		if (consonant_p(s, i))\n\
    			break;\n\
	    	i++;\n\
		}\n\
    	i++;\n\
	    n++;\n\
    	while(1) {\n\
		if (i > j)\n\
			return n;\n\
		if (!consonant_p(s, i))\n\
			break;\n\
		i++;\n\
		}\n\
	i++;\n\
	}\n\
}\n\
\n\
", TMP_BUFFER_SIZE, SACROSANCT_CHARS);


    fprintf(file, "\
/* Note that buffer should be at least TMP_BUFFER_SIZE */\n\
int stem(const char *string, char *buffer) {\n\
    size_t length;\n\
\n\
    strncpy(buffer, string, TMP_BUFFER_SIZE);\n\
    buffer[TMP_BUFFER_SIZE - 1] = '\\0';\n\
\n\
    length = strlen(buffer);\n\
\n\
");

    for (i = 0; i < count; i++) {
        if (measure(i) == SEPARATOR) {
            fprintf(file, "\
    }\n\
");
        } else {
            int from_len = strnlen(rule_from(i), RULE_STRING_MAX); 
            int to_len = strnlen(rule_to(i), RULE_STRING_MAX); 

            fprintf(file, "        if (m(buffer, length - 1) >= %d && strncmp(buffer + length - %d, \"%.*s\", %d) == 0 && length > %d) {\n",
                    measure(i), from_len, RULE_STRING_MAX, rule_from(i), RULE_STRING_MAX,
                    from_len - strnmatchlen(rule_from(i), rule_to(i), RULE_STRING_MAX) + SACROSANCT_CHARS);
            if (rule_to(i)[0] != '\0')
                fprintf(file, "            strncpy(buffer + length - %d, \"%.*s\", %d);\n",
                        from_len, RULE_STRING_MAX, rule_to(i), RULE_STRING_MAX);
            fprintf(file, "            buffer[length - %d] = '\\0';\n", from_len - to_len);
            if (to_len != from_len) {
                if (to_len > from_len) 
                    fprintf(file, "            length += %d;\n", to_len - from_len);
                else
                    fprintf(file, "            length -= %d;\n", from_len - to_len);
            }
            fprintf(file, "            break;\n");
            fprintf(file, "        } else ");
        }
    }

    fprintf(file, "\
    {}\n                                         \
    return SACROSANCT_CHARS;\n\
}\n\
");
    fclose(file);
}

/*
  Returns 1 if an individual is correctly formed.
*/
int GA_individual::sanity_check() {
    unsigned int i, j;

    if (count > (unsigned) MAX_INDIVIDUAL_SIZE)
        return 0;
    for (i = 0; i < count; i++) {
        if (measure(i) > MEASURE_MAX && 
            measure(i) != SEPARATOR)
            return 0;
        for (j = 0; j < (unsigned) RULE_STRING_MAX; j++) {
            if (*(rule_to(i) + j) < 'a' ||
                *(rule_to(i) + j) > 'z')
                if (*(rule_to(i) + j) != '\0')
                    return 0;
            if (*(rule_from(i) + j) < 'a' ||
                *(rule_from(i) + j) > 'z')
                if (*(rule_from(i) + j) != '\0')
                    return 0;
        }
    }

    return 1;
}

void GA_individual::keep_last_stats() {
	unsigned int i, together = 0;
	for (i = 0; i < count; i++)
		if (current_usage_stats[i]) {
			rule_usage_stats[i]++;
			together++;
		}
	rules_used_together[together]++;
}

void GA_individual::print_rule_usage(FILE *fd) {
	unsigned int i;
	for (i = 0; i < count; i++) {
		fprintf(fd, "%ld ", rule_usage_stats[i]);
	}
	fprintf(fd,"\n");
}

void GA_individual::clear_rule_usage() {
	unsigned int i;
	for (i = 0; i < count; i++) 
		rule_usage_stats[i] = 0;
}

