/*
	FENCE.H
	-------
*/
#ifndef FENCE_H_
#define FENCE_H_

#ifdef _MSC_VER
	#include <intrin.h>
#endif
/*
	ANT_WRITE_FENCE()
	-----------------
	This method ensures all writes are flushed to memory so that future reads
	from other threads get the value written.  Intel call it a fence.
*/
static inline void ANT_write_fence(void)
{
#ifdef _MSC_VER
	#ifdef __INTEL_COMPILER
		_mm_mfence();				// there is no write_barrier on the Intel compiler
	#else
		_WriteBarrier();
	#endif
#elif defined (__GNUC__)
	__sync_synchronize();
#else
	#error Unknown architecture - cannot insert memory barrier
#endif
}

#endif /* FENCE_H_ */
