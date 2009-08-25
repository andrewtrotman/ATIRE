#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "str.h"
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
   -------------
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
    while (**string == ' ' || **string == '\t' || **string == '\n') 
		(*string)++;
    if (**string == '\0') 
        return NULL;
    word = *string;
    while (**string != ' ' && **string != '\t' && **string != '\n' && **string != '\0')
        (*string)++;
    if (**string != '\0')
        *(*string)++ = '\0';
    return word;
}

int main(int argv, char **argc) {
    ANT *ant = ant_easy_init();
    char **query;
    long i, query_count, query_size;
	long long docs_returned;
    double idf, ictf;
    struct term_details_s t_d;
	struct collection_details_s c_d;

    ant_setup(ant);
	ant_get_collection_details(ant, &c_d);

    if (argv != 2)
        exit(fprintf(stderr, "Requires a single arg.\n"));

    query = get_queries(&query_count, argc[1]);

    printf("query_count: %ld\n", query_count);


    for (i = 0; i < query_count; i++) {
        char *term;
		ant_search(ant, &docs_returned, query[i]);
		puts(query[i]);
        term = eat_word(&query[i]); // Skip topic no.
        printf("Query %s: ", term);
        idf = query_size = 0;
		ictf = 1;
        while ((term = eat_word(&query[i]))) {
			double df, ctf;

            query_size++;
			ant_get_term_details(ant, term, &t_d);

			if (t_d.document_frequency > 0) {
				df = (double) c_d.documents_in_collection
					/ (double) t_d.document_frequency;
				ctf = (double) c_d.terms_in_collection
					/ (double) t_d.collection_frequency;

				printf("%s:%f, %f ", term, df, ctf);

				idf += df;
				ictf *= ctf;
			} else {
				ctf = 1.0;
				printf("%s:0.0, 0.0 ", term);
			}
        }
		printf("\n");
		printf("AvIDF: %lf\n", idf / query_size);
        printf("AvICTF: %lf\n", log(ictf) / log(2) / query_size);
        printf("Docs_returned: %lld\n", docs_returned);
		printf("QueryScope: %lf\n", 
			   log((double) docs_returned 
				   / (double) c_d.documents_in_collection));
    }

	ant_free(ant);

    return 0;
}
