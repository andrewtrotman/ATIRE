/*
	CRITICAL_SECTION.C
	------------------
*/
#ifdef _MSC_VER
	#include <windows.h>
	#define NT_CRITICAL_SECTIONS
#else
	#include <errno.h>
	#include <pthread.h>
#endif

#include <stdio.h>

#include "critical_section.h"
#include "critical_section_internals.h"

/*
	ANT_CRITICAL_SECTION::ANT_CRITICAL_SECTION()
	--------------------------------------------
*/
ANT_critical_section::ANT_critical_section()
{
internals = new ANT_critical_section_internals;
internals->level = 0;

#ifdef NT_CRITICAL_SECTIONS
	InitializeCriticalSection(&internals->mutex);
#else
	long err;

	err = pthread_mutex_init(&internals->mutex, NULL);
	if (err != 0)
		printf("ANT_critical_section::ANT_critical_section(): err=%ld\n", err);
#endif
}

/*
	ANT_CRITICAL_SECTION::~ANT_CRITICAL_SECTION()
	---------------------------------------------
*/
ANT_critical_section::~ANT_critical_section()
{
#ifdef NT_CRITICAL_SECTIONS
	DeleteCriticalSection(&internals->mutex);
#else
	long err;
	err = pthread_mutex_destroy(&internals->mutex);
	if (err != 0)
		printf("ANT_critical_section::~ANT_critical_section(): err=%ld\n", err);
#endif
delete internals;
}

/*
	ANT_CRITICAL_SECTION::ENTER()
	-----------------------------
*/
void ANT_critical_section::enter(void)
{
#ifdef NT_CRITICAL_SECTIONS
	EnterCriticalSection(&internals->mutex);
	internals->level++;
#else
	long err;
	err = pthread_mutex_lock(&internals->mutex);

	if (err != 0)
		printf("ANT_critical_section::enter(): err=%ld (%s)\n", err, err == EINVAL ? "EINVAL" : "?");
#endif
}

/*
	ANT_CRITICAL_SECTION::LEAVE()
	-----------------------------
*/
void ANT_critical_section::leave(void)
{
#ifdef NT_CRITICAL_SECTIONS
	internals->level--;
	LeaveCriticalSection(&internals->mutex);
#else
	long err;
	err = pthread_mutex_unlock(&internals->mutex);
	if (err != 0)
		printf("ANT_critical_section::leave(): err=%ld\n", err);
#endif
}
