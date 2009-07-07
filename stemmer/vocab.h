#ifndef _VOCAB_H_
#define _VOCAB_H_

const int MAX_TRIE_DEPTH = 10; 	/* Trie holds MAX_TRIE_DEPTH chars */
const int ALPHABET_SIZE = 26;
const int ARBITRARY_NUMBER = 300;

class ANT_search_engine;
class trie_node;

class Vocab {
public:
    Vocab(ANT_search_engine *);

    char *strgen();             // For strings without spaces
    char *strgen_2();           // For strings with empty string

	void print();
	void print_stats();
    void trim(int level);

private:
	trie_node *trie;
    char **strings;
    int string_count;

    char buffer[MAX_TRIE_DEPTH + 1];
    char *str_ptr;

    void roulette_wheel_string_builder(int);
};

#endif
