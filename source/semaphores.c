/*
	SEMAPHORES.C
	------------
*/
#ifdef _MSC_VER
	#include <windows.h>
#elif defined (__APPLE__)
	#include <mach/mach_init.h>
	#include <mach/mach_traps.h>
	#include <mach/semaphore.h>
	#include <mach/task.h>
#elif defined (sun)
       	#include <synch.h>
	typedef sema_t sem_t;
	#define sem_init(x, y, z)    sema_init(x, y, z, NULL)
	#define sem_wait(s)    sema_wait(s)
	#define sem_trywait(s) sema_trywait(s)
	#define sem_post(s)    sema_post(s)
	#define sem_destroy(s) sema_destroy(s)
	#define sched_yield()  thr_yield()
#else
	#include <semaphore.h>
#endif


#include "semaphores.h"
#include "semaphores_internals.h"

/*
	ANT_SEMAPHORES::ANT_SEMAPHORES()
	--------------------------------
*/
ANT_semaphores::ANT_semaphores(long initial, long maximum)
{
internals = new ANT_semaphores_internals;
#ifdef _MSC_VER
	internals->handle = CreateSemaphore(NULL, initial, maximum, NULL);
#elif defined (__APPLE__)
	semaphore_create(mach_task_self(), &internals->handle, SYNC_POLICY_FIFO, initial);
#else
	sem_init(&internals->handle, 0, initial);
#endif
}

/*
	ANT_SEMAPHORES::~ANT_SEMAPHORES()
	---------------------------------
*/
ANT_semaphores::~ANT_semaphores()
{
#ifdef _MSC_VER
	CloseHandle(internals->handle);
#elif defined(__APPLE__)
	semaphore_destroy(mach_task_self(), internals->handle);
#else
	sem_destroy(&internals->handle);
#endif
delete internals;
}

/*
	ANT_SEMAPHORES::ENTER()
	-----------------------
*/
void ANT_semaphores::enter(void)
{
#ifdef _MSC_VER
	WaitForSingleObject(internals->handle, INFINITE);
#elif defined(__APPLE__)
	semaphore_wait(internals->handle);
#else
	sem_wait(&internals->handle);
#endif
}

/*
	ANT_SEMAPHORES::LEAVE()
	-----------------------
*/
void ANT_semaphores::leave(void)
{
#ifdef _MSC_VER
	ReleaseSemaphore(internals->handle, 1, NULL);
#elif defined(__APPLE__)
	semaphore_signal(internals->handle);
#else
	sem_post(&internals->handle);
#endif
}

