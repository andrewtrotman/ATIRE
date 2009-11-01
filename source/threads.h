/*
	THREADS.H
	---------
*/
#ifndef THREADS_H_
#define THREADS_H_

#ifdef _MSC_VER
	typedef unsigned long ANT_thread_id_t;
#else
	#include <pthread.h>
	typedef pthread_t ANT_thread_id_t;
#endif

typedef void *ANT_thread_routine(void *);
void ANT_thread(ANT_thread_routine *start, void *parameter);

ANT_thread_id_t ANT_thread_id(void);


#endif /* THREADS_H_ */
