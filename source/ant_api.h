/*
 * ANT_API.H
 * ---------
 */

#ifndef __ANT_API_H__
#define __ANT_API_H__

#include "ant_params.h"

#ifdef  __cplusplus
extern "C" {
#endif

typedef void ANT;

/*
 * ANT_ANT_HANDLE
 */
//struct ANT_ant_handle;

ANT *ant_easy_init();

void ant_setup(ANT *ant);

ANT_ANT_params *ant_params(ANT *);

void ant_params_setup(ANT *ant);

long long ant_search(ANT *ant, char *query, char **docids);

void ant_stat(ANT *ant);

void ant_free(ANT *);

#ifdef  __cplusplus
}
#endif

#endif /* __ANT_API_H__ */
