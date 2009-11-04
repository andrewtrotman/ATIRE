/*
	SEMAPHORES_INTERNALS.H
	----------------------
*/
#ifndef SEMAPHORES_INTERNALS_H_
#define SEMAPHORES_INTERNALS_H_

/*
	class ANT_SEMAPHORES_INTERNALS
	------------------------------
*/
class ANT_semaphores_internals
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

#endif /* SEMAPHORES_INTERNALS_H_ */
