#include "memory.h"
#include "search_engine.h"
#include "search_engine_accumulator.h"
#include "search_engine_btree_leaf.h"
#include "ranking_function_bm25.h"
#include "ranking_function_lmd.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "str.h"
#include "stemmer_factory.h"
#include "ga_individual.h"
#include "ga_stemmer.h"

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

#define SCORES_RETAINED 10
struct query_data_s {
    long long docs_returned;
    double top_scores[SCORES_RETAINED];

    // top_doc_count = number of docs above (top_score * top_threshold)
    double top_threshold;
    long long top_doc_count;

	long query_size;
};

void perform_query(char *query, query_data_s *data, ANT_stemmer *stemmer) {
    ANT_search_engine_accumulator **results;
    long long results_list_length, current;
    int i;
    char *query_copy = strdup(query);
    char *query_ptr = query_copy;
    char *term = eat_word(&query_ptr);
	long long docs_returned;

    search_engine->init_accumulators();
	if (data)
		data->query_size = 0;
    while ((term = eat_word(&query_ptr))) {
        if (stemmer)
            search_engine->process_one_stemmed_search_term(stemmer, term, ranking_function);
        else
			search_engine->process_one_search_term(term, ranking_function);
		if (data)
			data->query_size++;
    }

	docs_returned = 0;
    results = search_engine->sort_results_list(LLONG_MAX, &docs_returned);

	if (data) {
		data->docs_returned = docs_returned;
		for (i = 0; i < SCORES_RETAINED && i < docs_returned; i++)
			data->top_scores[i] = (double) results[i]->get_rsv();
		for (; i < SCORES_RETAINED; i++)
			data->top_scores[i] = 0.0;

		data->top_doc_count = 0;

		if (!search_engine->results_list->is_zero_rsv(i))
			for (i = 0; i < docs_returned; i++) {
				if (data->top_scores[0] * data->top_threshold > (double) results[i]->get_rsv())
					break;
				data->top_doc_count++;
			}
	}
    free(query_copy);
}


void print_ga_stats(char *stemmer_file, char **query, int query_count) {
    int i;
	GA_stemmer *stemmer;
	GA_individual *individual;
	individual = new GA_individual();
	individual->load(stemmer_file);

	stemmer = new GA_stemmer(search_engine);
	stemmer->set_stemmer(individual);

	for (i = 0; i < query_count; i++) {
		individual->clear_rule_usage();
		perform_query(query[i], NULL, stemmer);
		individual->print_rule_usage(stdout);
	}
}


#define MAX_QUERY_LENGTH 100

int main(int argc, char **argv) {
	ANT_stemmer *stemmer;
    char **query;
	long i, j;
    long query_count, query_length, query_size;
    long long documents_in_collection, terms_in_collection;
	long long doc_set;
    double total_idf, product_ictf, max_idf, min_idf, total_scq, std_dev_idf, total_scs;
	double idfs[MAX_QUERY_LENGTH];
    ANT_search_engine_btree_leaf internal_details;
    struct query_data_s q_d, q_d_ws;

	/* Options */
	int do_top_scores = 0;
	int use_otago = 0;

	for (i = 0; i < MAX_QUERY_LENGTH; i++)
		idfs[i] = 0.0;

    /*
      Top scored docs are considered to be those with scores of (q_d.top_threshold * top doc) or greater. 
      This is entirely ranking function dependent (some functions might have a higher constant amount).

      NOTE - do this based on the difference between the highest and lowest scores?
     */
    q_d.top_threshold = 0.8;

/*
      N.B. : Averaged Similarity Collection Query - ctf = frequency, df = number of documents.

	  QL = n
      AvSCQ = 1/n SUM(q in Q) (1 + ln(ctf(q))) * ln(1 + |C|/df(q))
      AvSCS = 1/n SUM(q in Q) log2 (ictf(q) / n)
      AvQL = 1/n SUM(q in Q) strlen(q)
      AvIDF = 1/n SUM(q in Q) idf(q)
      MaxIDF = MAX(q in Q) idf(q)
      AvICTF = 1/n PRODUCT(q in Q) ictf(q)
      QueryScope = -log(|D| / |C|)

      D - docs returned
      C - collection
      Q - query
      q - query term
      n - |Q| 

      DocSet is wierd = (SUM(q in Q) df(q)) - DocsReturned 
      It's like which docs have at least 2 terms.

	  TopScore = the top BM25 Score
	  TopDocs = |d in D where score > TopScore * 0.8|

	  StdDevIDF(Gamma1) = StdDev(idfs)
	  Gamma2 = MaxIDF - MinIDF

	  dTopScore = (TopScore_ws - TopScore) / TopScore
	  dDocsReturned = (DocsReturned_ws - DocsReturned) / DocsReturned 
	  dTopDocs = (TopDocs_ws - TopDocs) / TopDocs 

	  X_ws = X with stemming
*/

    if (argc < 2) {
        fprintf(stderr, "Requires at least one arg.\n");
        fprintf(stderr, "Usage: %s <query file> [options]\n", argv[0]);
        fprintf(stderr, "Default is to produce some QPP scores\n");
        fprintf(stderr, "Options\n-------\n");
        fprintf(stderr, "t                Print top 10 scores w. stemming and w.o. stemming\n");
        fprintf(stderr, "tlm              Print top 10 scores w. stemming and w.o. stemming using language modeling\n");
		fprintf(stderr, "ga <filename>    Use a stemmer from the file and print rule usage stats\n");
		fprintf(stderr, "o                Use the otago stemmer instead of Porter\n");

        exit(-1);
	}

    ant_setup();
    documents_in_collection = search_engine->document_count();
    terms_in_collection = search_engine->get_collection_length();
    query = get_queries(&query_count, argv[1]);

	if (argc > 2) {
		if (strcmp(argv[2], "t") == 0) {
			do_top_scores = 1; 
		} else if (strcmp(argv[2], "ga") == 0) {
			if (argc < 4) { 
				fprintf(stderr, "This option requires another filename.\n");
				exit(-1);
			}
			print_ga_stats(argv[3], query, query_count);
			exit(0);
		} else if (strcmp(argv[2], "o") == 0) {
			use_otago = 1;
		} else if (strcmp(argv[2], "tlm") == 0) {
			do_top_scores = 1; 
			ranking_function = new ANT_ranking_function_lmd(search_engine);
		}
	}

	if (use_otago)
		stemmer = ANT_stemmer_factory::get_stemmer(ANT_stemmer_factory::OTAGO, search_engine);
	else
		stemmer = ANT_stemmer_factory::get_stemmer(ANT_stemmer_factory::PORTER, search_engine);

	if (do_top_scores) {
		for (i = 0; i < SCORES_RETAINED; i++)
			printf("NS%ld ", i + 1);
		for (i = 0; i < SCORES_RETAINED; i++)
			printf("S%ld ", i + 1);
		puts("");
	} else 
		puts("Query QL AvSCQ SCS AvQL AvIDF MaxIDF AvICTF DocsReturned TopDocs TopScore DocsReturnedWS TopDocsWS TopScoreWS DocSet QueryScope StdDevIDF Gamma2");

    for (i = 0; i < query_count; i++) {
        char *term;

		perform_query(query[i], &q_d, NULL);
		perform_query(query[i], &q_d_ws, stemmer);
        term = eat_word(&query[i]);
		if (!do_top_scores)
			printf("%s ", term);

        max_idf = 0;
		min_idf = documents_in_collection;
        total_idf = 0;
        query_size = 0;
        query_length = 0;
        total_scq = 0;
        total_scs = 0;
        product_ictf = 1;
        doc_set = 0;

        while ((term = eat_word(&query[i]))) {
            long document_frequency = 0;
            long collection_frequency = 0;
            double idf = 0.0;
            double ictf = 1.0;

            if (search_engine->get_postings_details(term, &internal_details)) {
                document_frequency = internal_details.document_frequency;
                collection_frequency = internal_details.collection_frequency;
            }

			/* 
			   PREVENT STOP WORDS! 
			   (For everything that doesn't rely on the queries being performed)
			*/
			if (document_frequency > documents_in_collection / 3) {
				doc_set += document_frequency; /* Still need this. */
				continue;
			}

            if (document_frequency > 0) {
                idf = (double) documents_in_collection
                    / (double) document_frequency;
                ictf = (double)terms_in_collection
                    / (double) collection_frequency;
            
                if (idf > max_idf) max_idf = idf;
                if (idf < min_idf) min_idf = idf;
                total_idf += idf;
                product_ictf *= ictf;
                total_scq += (1 + log(1 / ictf)) * log(1 + idf); 
                total_scs += log2(ictf / q_d.query_size); /* Must use q_d query_size! */
                doc_set += document_frequency;
				idfs[query_size] = idf;
            } else {
				idfs[query_size] = 0.0;
                ictf = 1.0;
                total_scs += log2(1.0 / q_d.query_size); /* Must use q_d query_size! */
            }

            query_length += strlen(term); 
            query_size++;
        }

		std_dev_idf = 0;
		for (j = 0; j < query_size; j++) {
			double tmp = ((total_idf / query_size) - idfs[j]);
			std_dev_idf += tmp * tmp;
		}
		std_dev_idf = sqrt(std_dev_idf / query_size);

		if (do_top_scores) {
			for (j = 0; j < SCORES_RETAINED; j++)
				printf("%lf ", q_d.top_scores[j] / 100);
			for (j = 0; j < SCORES_RETAINED; j++)
				printf("%lf ", q_d_ws.top_scores[j] / 100);
			puts("");
		} else {
			printf("%ld ", query_size); // QL
			printf("%lf ", (double) total_scq / (double) query_size); // AvSCQ
			printf("%lf ", (double) total_scs / (double) query_size); // SCS
			printf("%lf ", (double) query_length / (double) query_size); // AvQL
			printf("%lf ", total_idf / query_size); // AvIDF
			printf("%lf ", max_idf); // MaxIDF
			printf("%lf ", log2(product_ictf) / query_size); // AvICTF
			printf("%lld ", q_d.docs_returned); // DocsReturned
			printf("%lld ", q_d.top_doc_count); // TopDocs
			printf("%lf ", q_d.top_scores[0]); // TopScore
			printf("%lf ", (q_d_ws.docs_returned - q_d.docs_returned) / (double) q_d.docs_returned); // DocsReturnedWS
			printf("%lf ", (q_d_ws.top_doc_count - q_d.top_doc_count) / (double) q_d.top_doc_count); // TopDocsWS
			printf("%lf ", (q_d_ws.top_scores[0] - q_d.top_scores[0]) / (double) q_d.top_scores[0]); // TopScoreWS
			printf("%lld ", doc_set - q_d.docs_returned); // DocSet
			printf("%lf ", 
				   -log((double) q_d.docs_returned 
						/(double) documents_in_collection)); // QueryScope
			printf("%lf ", std_dev_idf); // StdDevIDF
			printf("%lf\n", max_idf / min_idf); // Gamma2
		}
    }

    return 0;
}
