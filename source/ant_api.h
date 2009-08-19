/*
 * ANT_API.H
 * ---------
 */

#ifndef __ANT_API_H__
#define __ANT_API_H__

#include "ant_params.h"

struct term_details_s
{
    long long documents_in_collection;
	long long collection_frequency;
	long document_frequency;
} ;

#ifdef  __cplusplus
extern "C" {
#endif

typedef void ANT;

ANT *ant_easy_init();

void ant_setup(ANT *ant);

ANT_ANT_params *ant_params(ANT *ant);

void ant_post_processing_stats_init(ANT *ant);

char **ant_search(ANT *ant, long long *hits, char *query, long topic_id = -1);

struct term_details_s *ant_get_term_details(ANT *ant, char *term);

double ant_cal_map(ANT *ant);

void ant_stat(ANT *ant);

void ant_free(ANT *ant);

#ifdef  __cplusplus
}
#endif

#endif /* __ANT_API_H__ */
