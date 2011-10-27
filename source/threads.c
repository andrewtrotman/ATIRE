/*
	THREAD.C
	--------
*/
#ifdef _MSC_VER
	#include <windows.h>
	#include <process.h>
#else
	#include <pthread.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "threads.h"

#ifdef _MSC_VER
	typedef void ANT_NT_thread_routine(void *);
#else
	extern "C" { typedef void *ANT_unix_thread_routine(void *); }
#endif

/*
	ANT_THREAD()
	------------
*/
void ANT_thread(ANT_thread_routine *start, void *parameter)
{
static long threads_started = 0;

#ifdef _MSC_VER
	uintptr_t got;
	got = _beginthread((ANT_NT_thread_routine *)start, 0, parameter);
	if (got == -1)
		exit(printf("Can't start thread"));
#else
	long got;
	pthread_t thread_id;
	pthread_attr_t attr;
	long err1, err2, err3;

	err1 = pthread_attr_init(&attr);
	err2 = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	got = pthread_create(&thread_id, &attr, (ANT_unix_thread_routine *)start, parameter);
	err3 = pthread_attr_destroy(&attr);
	if (got != 0 || err1 != 0 || err2 != 0 || err3 != 0)
		exit(printf("Can't start thread (returned:%ld) err1=%ld err2=%ld err3=%ld (threads used:%ld)\n", got, err1, err2, err3, threads_started));
	threads_started++;
#endif
}

/*
	ANT_THREAD_ID()
	---------------
*/
ANT_thread_id_t ANT_thread_id(void)
{
#ifdef _MSC_VER
	return GetCurrentThreadId();
#else
	return pthread_self();
#endif
}

