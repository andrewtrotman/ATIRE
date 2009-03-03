#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "strnlen.h"
#include "ga_individual.h"

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
    unsigned int i, skipping = FALSE;
    int length;
    static char buffer[TMP_BUFFER_SIZE];
    
    strncpy(buffer, string, TMP_BUFFER_SIZE);
    buffer[TMP_BUFFER_SIZE - 1] = '\0';
    length = strlen(buffer);

    for (i = 0; i < count; i++) {
        if (measure(i) == SEPARATOR) {
            /* Stop skipping at separator */
            skipping = FALSE;
        } else if (!skipping) {
            /* Check that rule can be applied */
            int from_len = strnlen(rule_from(i), RULE_STRING_MAX); 

            if (m(buffer, length - 1) >= measure(i) &&
                strncmp(buffer + length - from_len,
                        rule_from(i), RULE_STRING_MAX) == 0) {

                int to_len = strnlen(rule_to(i), RULE_STRING_MAX); 

                /* Ensure that the first SACROSANCT_CHARS are respected */ 
                if (length - from_len + 
                    strnmatchlen(rule_from(i), rule_to(i), RULE_STRING_MAX) < SACROSANCT_CHARS)
                    continue;

                strncpy(buffer + length - from_len, rule_to(i), RULE_STRING_MAX);

                buffer[length - from_len + to_len] = '\0';
                length += to_len - from_len;

                /*
                  TODO: 
                  only use strnlen once.
                */

                skipping = TRUE;
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
        if (i % RULE_SIZE == 0) putchar(' ');
        if (rules[i] >= 'a' && rules[i] <= 'z')
            putchar(rules[i]);
        else if (rules[i] >= 0 && rules[i] <= 9)
            putchar(rules[i] + '0');
        else
            putchar('-');
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

void GA_individual::mutate(GA_individual *c, char *(*strgen)(), char *(*strgen_2)()) {
    c->generate(strgen, strgen_2);
    if (rand() % 2 == 0) 
        this->crossover(c,c);
    else 
        c->crossover(this,c);
}

/* 
   Places a new individual into c based on the parents p1 & p2.
*/
void GA_individual::crossover(GA_individual *p2, GA_individual *c) {
    unsigned int point = random_from(0, this->rules_size());
    /* Ensure the second point shares the mid-rule position of the first */
    unsigned int min_point2 = ((signed)p2->count + (signed)point / RULE_SIZE - MAX_RULES > 0) ? 
        (p2->count + point / RULE_SIZE - MAX_RULES) : 0;
    unsigned int point2 = random_from(min_point2, p2->count) * RULE_SIZE
        + (point % RULE_SIZE);

    /* Still need to copy if p2 is also c, as the part from p1 may be small (i.e. p2 needs to shift)
     * This is also why it is done first. */
    memmove(c->rules + point, p2->rules + point2, p2->rules_size() - point2);
    if (this != c)
        memcpy(c->rules, this->rules, point);
    c->is_evaluated = FALSE;
    c->count = (point + (p2->rules_size() - point2)) / RULE_SIZE;
}

/* 
   Generates new individuals using strings from a string gen function.
   No allocation or deallocation (or modification) is done on the strings
   ONLY POINTERS ARE NEEDED.

   Strings will be used until null-termed or over 6 chars
*/
void GA_individual::generate(char *(*strgen)(), char *(*strgen_2)()) {
    unsigned int i;
    count = (unsigned int) rand() % (MAX_RULES) + 1;
    memset(rules, '\0', count * RULE_SIZE);

    for (i = 0; i < count; i++) {
        rules[i * RULE_SIZE] = (char) (rand() % (MEASURE_MAX + 1));
        if (rules[i * RULE_SIZE] == MEASURE_MAX)
            rules[i * RULE_SIZE] = SEPARATOR;

        strncpy(rules + (i * RULE_SIZE) + 1,
                strgen(),
                RULE_STRING_MAX);
        strncpy(rules + (i * RULE_SIZE) + RULE_STRING_MAX + 1,
                strgen_2(),
                RULE_STRING_MAX);
    }
}

void GA_individual::load(char *filename) {
    char buffer[1024];
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
char *stem(const char *string, char *buffer) {\n\
    int length;\n\
    strncpy(buffer, string, TMP_BUFFER_SIZE);\n\
    buffer[TMP_BUFFER_SIZE - 1] = '\\0';\n\
\n\
    length = strlen(buffer);\n\
\n\
    do {\n\
");

    for (i = 0; i < count; i++) {
        if (measure(i) == SEPARATOR) {
            fprintf(file, "\
    } while (0);\n\
    do {\n\
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
            fprintf(file, "        }\n");
        }
    }

    fprintf(file, "\
    } while (0);\n\
    return buffer;\n\
}\n\
");
    fclose(file);
}
