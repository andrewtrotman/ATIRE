#ifndef _VOCAB_H_
#define _VOCAB_H_

class ANT_search_engine;
class trie_node;

class Vocab {
public:
    Vocab(ANT_search_engine *);

    char *strgen();             // For strings without spaces
    char *strgen_2();           // For strings with empty string

	void print();
    void weight_strings();

private:
	trie_node *trie;
    char **strings;
    int string_count;
};

#endif
