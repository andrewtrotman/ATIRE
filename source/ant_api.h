/*
 * ANT_API.H
 * ---------
 */

#ifndef __ANT_API_H__
#define __ANT_API_H__

#ifdef  __cplusplus
extern "C" {
#endif

typedef void ANT;

/*
 * ANT_ANT_HANDLE
 */
//struct ANT_ant_handle;

ANT *ant_init();
long long ant_search(ANT *ant, char *query);
void ant_free(ANT *);

#ifdef  __cplusplus
}
#endif

#endif /* __ANT_API_H__ */
