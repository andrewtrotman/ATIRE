/*
	EVENT.C
	-------
*/
#ifdef _MSC_VER
	#define NT_EVENTS
	#include <windows.h>
#else
	#include <pthread.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include "event.h"
#include "event_internals.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_EVENT::ANT_EVENT()
	----------------------
*/
ANT_event::ANT_event()
{
internals = new ANT_event_internals;
#ifdef NT_EVENTS
	internals->event = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (internals->event == NULL)
		puts("CreateEvent failure");
#else
	long err1, err2;

	err1 = pthread_cond_init(&internals->event, NULL);
	err2 = pthread_mutex_init(&internals->mutex, NULL);
	if (err1 != 0 || err2 != 0)
		printf("ANT_event::ANT_event(): err1:%ld err2:%ld\n", err1, err2);
#endif
internals->val = FALSE;
}

/*
	ANT_EVENT::~ANT_EVENT()
	-----------------------
*/
ANT_event::~ANT_event()
{
#ifdef NT_EVENTS
	CloseHandle(internals->event);
#else
	long err1, err2;

	err1 = pthread_mutex_destroy(&internals->mutex);
	err2 = pthread_cond_destroy(&internals->event);
	if (err1 != 0 || err2 != 0)
		printf("ANT_event::~ANT_event(): err1:%ld err2:%ld\n", err1, err2);
	
#endif
delete internals;
}

/*
	ANT_EVENT::SIGNAL()
	-------------------
*/
void ANT_event::signal(void)
{
#ifdef NT_EVENTS
	internals->val = TRUE;
	SetEvent(internals->event);
#else
	long err1, err2, err3;

	err1 = pthread_mutex_lock(&internals->mutex);
	internals->val = TRUE;
	err2 = pthread_cond_signal(&internals->event);
	err3 = pthread_mutex_unlock(&internals->mutex);

	if (err1 != 0 || err2 != 0 || err3 != 0)
		printf("ANT_event::signal(): err1:%ld err2:%ld err3:%ld\n", err1, err2, err3);
#endif
}

/*
	ANT_EVENT::CLEAR()
	------------------
*/
void ANT_event::clear(void)
{
#ifdef NT_EVENTS
	internals->val = FALSE;
	ResetEvent(internals->event);
#else
	long err1, err2;

	err1 = pthread_mutex_lock(&internals->mutex);
	internals->val = FALSE;
	err2 = pthread_mutex_unlock(&internals->mutex);

	if (err1 != 0 || err2 != 0)
		printf("ANT_event::clear(): err1:%ld err2:%ld\n", err1, err2);
#endif
}

/*
	ANT_EVENT::WAIT()
	-----------------
*/
void ANT_event::wait(void)
{
#ifdef NT_EVENTS
	WaitForSingleObject(internals->event, INFINITE);
#else
	long err, err1, err2;
	err1 = pthread_mutex_lock(&internals->mutex);

	while (internals->val == FALSE)
		{
		err = pthread_cond_wait(&internals->event, &internals->mutex);
		if (err != 0)
			printf("ANT_event::wait(): err:%ld\n", err);
		}

	err2 = pthread_mutex_unlock(&internals->mutex);

	if (err1 != 0 || err2 != 0)
		printf("ANT_event::wait(): err1:%ld err2:%ld\n", err1, err2);
#endif
}

/*
	ANT_EVENT::POLL()
	-----------------
*/
long ANT_event::poll(void)
{
#ifdef NT_EVENTS
	return WaitForSingleObject(internals->event, 0) == WAIT_OBJECT_0 ? 1 : 0;
#else
	long got;
	long err1, err2;

	err1 = pthread_mutex_lock(&internals->mutex);
	got = internals->val;
	err2 = pthread_mutex_unlock(&internals->mutex);

	if (err1 != 0 || err2 != 0)
		printf("ANT_event::poll(): err1:%ld err2:%ld\n", err1, err2);

	return got;
#endif
}

/*
	ANT_EVENT::PULSE()
	------------------
*/
void ANT_event::pulse(void)
{
#ifdef NT_EVENTS
	PulseEvent(internals->event);
#else
	exit(printf("ANT_event::pulse() Not written yet (except for Windows)\n"));
#endif
}
