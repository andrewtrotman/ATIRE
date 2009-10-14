#include "memory.h"
#include "search_engine.h"
#include "search_engine_accumulator.h"
#include "search_engine_btree_leaf.h"
#include "ranking_function_bm25.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "str.h"

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

ANT_memory *memory;
ANT_search_engine *search_engine;
ANT_ranking_function *ranking_function;

void ant_setup() {
    memory = new ANT_memory();
	search_engine = new ANT_search_engine(memory);
    ranking_function = new ANT_ranking_function_BM25(search_engine, 0.9, 0.4);
    search_engine->set_trim_postings_k(LLONG_MAX);
}

struct query_data_s {
    long long docs_returned;
    double top_score;

    // top_doc_count = number of docs above (top_score * top_threshold)
    double top_threshold;
    long long top_doc_count;
};

void perform_query(char *query, query_data_s *data, ANT_stemmer *stemmer) {
    ANT_search_engine_accumulator **results_list;
    long long results_list_length, current;
    char *query_copy = strdup(query);
    char *query_ptr = query_copy;
    char *term = eat_word(&query_ptr);

    search_engine->init_accumulators();

    while ((term = eat_word(&query_ptr))) {
        if (stemmer)
            search_engine->process_one_stemmed_search_term(stemmer, term, ranking_function);
        else
            search_engine->process_one_search_term(term, ranking_function);
    }
    
    data->docs_returned = 0;
    search_engine->sort_results_list(LLONG_MAX, &data->docs_returned);
    results_list = search_engine->get_accumulator_pointers();
    results_list_length = search_engine->document_count();

    data->top_score = (double) results_list[0]->get_rsv();
    data->top_doc_count = 0;


    if (!results_list[0]->is_zero_rsv())
        for (current = 0; current < results_list_length; current++) {
            if (data->top_score * data->top_threshold > (double) results_list[current]->get_rsv())
                break;
            data->top_doc_count++;
        }
    
    free(query_copy);
}

int main(int argv, char **argc) {
    char **query;
    long i, query_count, query_size, query_length;
	long long doc_set;
    long long documents_in_collection, terms_in_collection;
    double total_idf, product_ictf, max_idf, total_scq;
    ANT_search_engine_btree_leaf internal_details;
    struct query_data_s q_d;

    /*
      Top scored docs are considered to be those with scores 0.8 of the top doc. 
      This is entirely score dependent (some scores might have a higher constant amount).

      NOTE - do this based on the difference between the highest and lowest scores?
     */
    q_d.top_threshold = 0.8;

/*
      N.B. : Averaged Similarity Collection Query - ctf = frequency, df = number of documents.

      QL = n
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

      DocSet is wierd = (SUM(q in Q) df(q)) - DocsReturned 
      It's like which docs have at least 2 terms.
      TopScore = top BM25 score
      TopDocs = Docs Returned above 0.8 * top BM25 Score

*/

    ant_setup();
    documents_in_collection = search_engine->document_count();
    terms_in_collection = search_engine->get_collection_length();


    if (argv != 2)
        exit(fprintf(stderr, "Requires a single arg.\n"));

    query = get_queries(&query_count, argc[1]);

    printf("query_count: %ld\n", query_count);

    for (i = 0; i < query_count; i++) {
        char *term;

		perform_query(query[i], &q_d, NULL);
        term = eat_word(&query[i]); // Skip topic no.
        printf("Query %s: ", term);
        max_idf = 0;
        total_idf = 0;
        query_size = 0;
        query_length = 0;
        total_scq = 0;
        product_ictf = 1;
        doc_set = 0;

        while ((term = eat_word(&query[i]))) {
            long document_frequency = 0;
            long collection_frequency = 0;
            double idf = 0.0;
            double ictf = 1.0;
            query_length += strlen(term); 
            query_size++;
            
            if (search_engine->get_postings_details(term, &internal_details)) {
                document_frequency = internal_details.document_frequency;
                collection_frequency = internal_details.collection_frequency;
            }

            if (document_frequency > 0) {
                idf = (double) documents_in_collection
                    / (double) document_frequency;
                ictf = (double)terms_in_collection
                    / (double) collection_frequency;
            
                printf("%s:%f, %f ", term, idf, ictf);

                if (idf > max_idf) max_idf = idf;
                total_idf += idf;
                product_ictf *= ictf;
                total_scq += (1 + log(1 / ictf)) * log(1 + idf); 
                doc_set += document_frequency;
            } else {
                ictf = 1.0;
                printf("%s:0.0, 0.0 ", term);
            }
        }

        printf("\n");
        printf("QuerySize: %ld\n", query_size);
        printf("AvSCQ: %lf\n", (double) total_scq / (double) query_size);
        printf("AvQL: %lf\n", (double) query_length / (double) query_size);
        printf("AvIDF: %lf\n", total_idf / query_size);
        printf("MaxIDF: %lf\n", max_idf);
        printf("AvICTF: %lf\n", log2(product_ictf) / query_size);
        printf("Docs_returned: %lld\n", q_d.docs_returned);
        printf("TopDocs: %lld\n", q_d.top_doc_count);
        printf("TopScore: %lf\n", q_d.top_score);
        printf("DocSet: %lld\n", doc_set - q_d.docs_returned);
        printf("QueryScope: %lf\n", 
               log((double) q_d.docs_returned 
                   / (double) documents_in_collection));
    }

    return 0;
}
