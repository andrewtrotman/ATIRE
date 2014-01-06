/*
	FENCE.H
	-------
*/
#ifndef FENCE_H_
#define FENCE_H_

#ifdef _MSC_VER
	#include <windows.h>
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
		#ifdef NEVER
			_WriteBarrier();		// Visual Studio 2008 states that this should be used but the 2010 docs state that is was wrong.
		#else
			MemoryBarrier();
		#endif
	#endif
#elif defined (__GNUC__)
	#if (__GNUC__) >= 4
		__sync_synchronize();
	#else
		#error Unknown version of GCC - cannot insert memory barrier
	#endif
#else
	#error Unknown architecture - cannot insert memory barrier
#endif
}

#ifdef _MSC_VER
#define ANT_compare_and_swap(a, b, c) InterlockedCompareExchangePointer((volatile PVOID *)(a), (b), (c))
#else
#define ANT_compare_and_swap(a, b, c) ((void)__sync_val_compare_and_swap(a, c, b))
#endif

#endif /* FENCE_H_ */
