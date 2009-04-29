#include <string.h>
#include <stdlib.h>
#include "vocab.h"
#include "btree_iterator.h"
#include "ga_individual.h"

const int MAX_TRIE_DEPTH = 4;
const int ALPHABET_SIZE = 26;

/* I store a trie, starting from the endings of words.
 * up to a maximum depth. Each node contains the cumulative
 * frequency of each occurrance. 
 */
class trie_node {
    public:
        trie_node *child[ALPHABET_SIZE];
        int cum_freq;
        
        trie_node() {
            int i;
            for (i = 0; i < ALPHABET_SIZE; i++)
                this->child[i] = NULL;
            this->cum_freq = 0;
        } 
        /*
         * In general:
         * trie->add(word, strlen(word), 0);
         */
        void add(char *word, int pos, int depth) {
            this->cum_freq++; 
            
            if (pos == 0)
                return;
            if (depth >= MAX_TRIE_DEPTH)
                return;
            if (this->child[word[pos-1] - 'a'] == NULL)
                this->child[word[pos-1] - 'a'] = new trie_node();
            this->child[word[pos-1] - 'a']->add(word, pos - 1, depth + 1);
        }

        void print(char me, int depth) {
            int i;
            for (i = 0; i < depth; i++)
                putchar(' ');
            printf("'%c' * %d\n", me, this->cum_freq);
            for (i = 0; i < ALPHABET_SIZE; i++) {
                if (this->child[i])
                    this->child[i]->print(i + 'a', depth + 1);
            }
        }

};

/*
  TODO: Use the trie (i.e. Trie roulette wheel)
 */
Vocab::Vocab(ANT_search_engine *search_engine) {
    char *term;
    int i;
    ANT_btree_iterator iterator(search_engine);
    
    this->strings = NULL;
    this->string_count = 0;

    for (string_count = 0, iterator.first(NULL); iterator.next() != NULL; string_count++)
        ;

    strings = (char **) malloc(sizeof(strings[0]) * string_count);
    for (i = 0, term = iterator.first(NULL); term != NULL; term = iterator.next(), i++)
        strings[i] = strdup(term);
}

void Vocab::weight_strings() {
}

/* Does not allocate a string, do not free */
char *Vocab::strgen() {
    char *str = strings[rand() % string_count];
    int len = strlen(str);
    str += len;
    if (len > RULE_STRING_MAX)
        len = RULE_STRING_MAX;
    return str - (rand() % len + 1);
}

/* Does not allocate a string, do not free */
char *Vocab::strgen_2() {
    char *str = strings[rand() % string_count];
    int len = strlen(str);
    str += len;
    if (len > RULE_STRING_MAX)
        len = RULE_STRING_MAX;
    return str - rand() % (len + 1);
}
