/*
	SEMAPHORE.C
	-----------
*/
#ifdef _MSC_VER
	#include <windows.h>
#else
	#include <semaphore.h>
#endif
#include "semaphore.h"
#include "semaphore_internals.h"

/*
	ANT_SEMAPHORE::ANT_SEMAPHORE()
	------------------------------
*/
ANT_semaphore::ANT_semaphore(long initial, long maximum)
{
internals = new ANT_semaphore_internals;
#ifdef _MSC_VER
	internals->handle = CreateSemaphore(NULL, initial, maximum, NULL);
#else
	sem_init(&internals->handle, 0, initial);
#endif
}

/*
	ANT_SEMAPHORE::~ANT_SEMAPHORE()
	-------------------------------
*/
ANT_semaphore::~ANT_semaphore()
{
#ifdef _MSC_VER
	CloseHandle(internals->handle);
#else
	sem_destroy(&internals->handle);
#endif
delete internals;
}

/*
	ANT_SEMAPHORE::ENTER()
	----------------------
*/
void ANT_semaphore::enter(void)
{
#ifdef _MSC_VER
	WaitForSingleObject(internals->handle, INFINITE);
#else
	sem_wait(&internals->handle);
#endif
}

/*
	ANT_SEMAPHORE::LEAVE()
	----------------------
*/
void ANT_semaphore::leave(void)
{
#ifdef _MSC_VER
	ReleaseSemaphore(internals->handle, 1, NULL);
#else
	sem_post(&internals->handle);
#endif
}

/*
	ANT_SEMAPHORE::POLL()
	---------------------
*/
long ANT_semaphore::poll(void)
{
#ifdef _MSC_VER
	/*
		Windows doesn't have a way to get the value of the semaphore so we poll the semaphore
		with immediate timeout and then return it to its initial state
	*/
	long state;

	if (WaitForSingleObject(internals->handle, 0) == WAIT_TIMEOUT)
		state = 0;
	else
		{
		ReleaseSemaphore(internals->handle, 1, &state);
		state++;		// +1 because ReleaseSemaphore return the previous value
		}
#else
	int state;

	sem_getvalue(&internals->handle, &state);
#endif

return state;
}
