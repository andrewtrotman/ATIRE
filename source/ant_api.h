/*
 * ANT_API.H
 * ---------
 */

#ifndef ANT_API_H_
#define ANT_API_H_

#include "ant_params.h"

struct term_details_s
{
	long long collection_frequency;
	long document_frequency;
} ;

struct collection_details_s
{
	long long documents_in_collection;
	long long terms_in_collection;
} ;

#ifdef  __cplusplus
extern "C" {
#endif

typedef void ANT;

ANT *ant_easy_init();

void ant_setup(ANT *ant);

void ant_filenames_init(ANT* ant, const char *doclist_filename = "doclist.aspt", const char *index_filename = "index.aspt");

ANT_ANT_params *ant_params(ANT *ant);

void ant_post_processing_stats_init(ANT *ant);

char **ant_search(ANT *ant, long long *hits, char *query, const char *topic_id = ""/*long topic_id = -1*/, long boolean = 0);

struct collection_details_s *ant_get_collection_details(ANT *ant, struct collection_details_s *collection_details);

struct term_details_s *ant_get_term_details(ANT *ant, char *term, struct term_details_s *term_details);

double ant_cal_map(ANT *ant);

void ant_stat(ANT *ant);

void forum_output(ANT *ant, const char *topic_id/*long topic_id*/, long long hits);

void ant_free(ANT *ant);

#ifdef  __cplusplus
}
#endif

#endif  /* ANT_API_H_ */
