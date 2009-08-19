#include <stdio.h>
#include "str.h"
#include <stdlib.h>
#include "ant_api.h"

/*
	class FILE_ITERATOR
	-------------------
*/
class file_iterator
{
private:
	FILE *fp;
	char query[1024];

public:
	file_iterator(char *filename)
		{
		if (filename == NULL)
			fp = stdin;
		else
			fp = fopen(filename, "rb");
		if (fp == NULL)
			exit(printf("Cannot open topic file:'%s'\n", filename));
		}
	~file_iterator() { if (fp != NULL) fclose(fp); }
	char *first(void) { 
        fseek(fp, 0, SEEK_SET);
        return fgets(query, sizeof(query), fp); 
    }
	char *next(void) { return fgets(query, sizeof(query), fp); }
} ;

/* 
   GET_QUERIES()
*/
char **get_queries(long *query_count, char *query_filename) {
    file_iterator input(query_filename);
	char **queries;
    char *query;
    int i;
    *query_count = 0;
    for (query = input.first(); query != NULL; query = input.next()) {
        strip_space_inplace(query);
        if (query != '\0')
            (*query_count)++;
    }
    queries = (char **)malloc(sizeof *queries * *query_count);
    for (query = input.first(), i = 0; query != NULL; query = input.next(), i++) {
        strip_space_inplace(query);
        if (query == '\0')
            continue;

        queries[i] = strnew(query);
    }
    return queries;
}

char *eat_word(char **string) {
    char *word;
    while (**string == ' ' || **string == '\t' || **string == '\n') (*string)++;
    if (**string == '\0') 
        return NULL;
    word = *string;
    while (**string != ' ' && **string != '\t' && **string != '\n' && **string != '\0') {
        (*string)++;
    }
    if (**string != '\0')
        *(*string)++ = '\0';
    return word;
}

int main(int argv, char **argc) {
    ANT *ant = ant_easy_init();
    char **query;
    long i, query_count, query_size;
    double idf;
    struct term_details_s *t_d;

    ant_setup(ant);

    if (argv != 2)
        exit(fprintf(stderr, "Requires a single arg.\n"));

    query = get_queries(&query_count, argc[1]);
    printf("query_count: %ld\n", query_count);
    for (i = 0; i < query_count; i++) {
        char *term;
        term = eat_word(query + i); // Skip topic number
        printf("Query %s: ", term);
        idf = query_size = 0;
        while ((term = eat_word(query + i))) {
            query_size++;
            t_d = ant_get_term_details(ant, term);

            idf += (double) t_d->documents_in_collection
                / (double) t_d->document_frequency;
            printf("%s:%f ", term, (double) t_d->documents_in_collection
                / (double) t_d->document_frequency);
        }
        printf("AvIDF: %lf\n", idf);
    }

    return 0;
}
