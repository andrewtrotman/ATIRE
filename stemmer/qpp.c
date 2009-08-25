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
    long i, query_count, query_size, query_length;
	long long docs_returned;
    double total_idf, product_ictf, max_idf, total_scq;
    struct term_details_s t_d;
	struct collection_details_s c_d;

	/*
      N.B. : Averaged Similarity Collection Query - ctf = frequency, df = number of documents.

      AvSCQ = 1/n SUM(q in Q) (1 + ln(ctf(q))) * ln(1 + |C|/df(q))
      AvQL = 1/n SUM(q in Q) strlen(q)
      AvIDF = 1/n SUM(q in Q) idf(q)
      MaxIDF = MAX(q in Q) idf(q)
      AvICTF = 1/n PRODUCT(q in Q) ictf(q)
      QueryScope = |D| / |C|

      D - docs returned
      C - collection
      Q - query
      q - query term
      n - |Q| 
	*/

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
        max_idf = 0;
        total_idf = 0;
        query_size = 0;
        query_length = 0;
        total_scq = 0;
        product_ictf = 1;

        while ((term = eat_word(&query[i]))) {
            double idf = 0.0;
            double ictf = 1.0;
            query_length += strlen(term); 
            query_size++;
            ant_get_term_details(ant, term, &t_d);

            if (t_d.document_frequency > 0) {
                idf = (double) c_d.documents_in_collection
                    / (double) t_d.document_frequency;
                ictf = (double) c_d.terms_in_collection
                    / (double) t_d.collection_frequency;
            
                printf("%s:%f, %f ", term, idf, ictf);

                if (idf > max_idf) max_idf = idf;
                total_idf += idf;
                product_ictf *= ictf;
                total_scq += (1 + log(1 / ictf)) * log(1 + idf); 
            } else {
                ictf = 1.0;
                printf("%s:0.0, 0.0 ", term);
            }
        }

        printf("\n");
        printf("AvSCQ: %lf\n", (double) total_scq / (double) query_size);
        printf("AvQL: %lf\n", (double) query_length / (double) query_size);
        printf("AvIDF: %lf\n", total_idf / query_size);
        printf("MaxIDF: %lf\n", max_idf);
        printf("AvICTF: %lf\n", log2(product_ictf) / query_size);
        printf("Docs_returned: %lld\n", docs_returned);
        printf("QueryScope: %lf\n", 
               log((double) docs_returned 
                   / (double) c_d.documents_in_collection));
    }

	ant_free(ant);

    return 0;
}
