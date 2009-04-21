#ifndef _VOCAB_H_
#define _VOCAB_H_

class ANT_search_engine;

class vocab {
public:
    vocab(ANT_search_engine *);

    char *strgen();             // For strings without spaces
    char *strgen_2();           // For strings with empty string

private:
    char **strings = NULL;
    int string_count = 0;
};

#endif
