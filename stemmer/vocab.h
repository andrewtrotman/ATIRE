#ifndef _VOCAB_H_
#define _VOCAB_H_

class ANT_search_engine;

class Vocab {
public:
    Vocab(ANT_search_engine *);

    char *strgen();             // For strings without spaces
    char *strgen_2();           // For strings with empty string

    void weight_strings();

private:
    char **strings;
    int string_count;
};

#endif
