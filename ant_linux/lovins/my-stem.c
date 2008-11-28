#include <string.h>

extern char *stem (char *word);

char *lovins(char *in_word) {
    char word[256];

    word[0] = strlen(in_word);
    strncpy(word + 1, in_word, word[0]);
    stem(word);
    strncpy(in_word, word + 1, word[0]);
    in_word[word[0]] = 0;

    return in_word;
}
