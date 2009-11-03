/*
	SEMAPHORE_INTERNALS.H
	---------------------
*/
#ifndef SEMAPHORE_INTERNALS_H_
#define SEMAPHORE_INTERNALS_H_

/*
	class ANT_SEMAPHORE_INTERNALS
	-----------------------------
*/
class ANT_semaphore_internals
{
public:
#ifdef _MSC_VER
	HANDLE handle;
#elif defined (__APPLE__)
	semaphore_t handle;
#else
	sem_t handle;
#endif
} ;

#endif /* SEMAPHORE_INTERNALS_H_ */
