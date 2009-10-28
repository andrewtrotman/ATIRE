/*
	THREADS.H
	---------
*/
#ifndef THREADS_H_
#define THREADS_H_

typedef void *ANT_thread_routine(void *);

void ANT_thread(ANT_thread_routine *start, void *parameter);
long ANT_thread_id(void);


#endif /* THREADS_H_ */
