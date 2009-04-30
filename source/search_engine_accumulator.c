/*
	SEARCH_ENGINE_ACCUMULATOR.C
	---------------------------
	The Implementation of top-k-sort is based on the qsort implementation in the C Runtime Library shipped with Microsoft Visual Studio 2008
*/
#include <stdlib.h>
#include "search_engine_accumulator.h"

#define CUTOFF 8 		// drop from qsort to insertion sort when numelements is this value
#define STKSIZ (8 * sizeof(void *) - 2)		// number of stack elements needed to sort the array

/*
	ANT_SEARCH_ENGINE_ACCUMULATOR::COMPARE()
	----------------------------------------
*/
int ANT_search_engine_accumulator::compare(const void *a, const void *b)
{
ANT_accumulator_t sign;

sign = ((ANT_search_engine_accumulator *)a)->rsv - ((ANT_search_engine_accumulator *)b)->rsv;
return sign < 0 ? 1 : sign > 0 ? -1 : 0;
}

/*
	ANT_SEARCH_ENGINE_ACCUMULATOR::COMPARE_POINTER()
	------------------------------------------------
*/
int ANT_search_engine_accumulator::compare_pointer(const void *a, const void *b)
{
ANT_accumulator_t sign;

sign = (*(ANT_search_engine_accumulator **)a)->rsv - (*(ANT_search_engine_accumulator **)b)->rsv;
return sign < 0 ? 1 : sign > 0 ? -1 : 0;
}

/*
	ANT_SEARCH_ENGINE_ACCUMULATOR::COMPARE_POINTER()
	------------------------------------------------
*/
inline int ANT_search_engine_accumulator::compare_pointer(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b)
{
ANT_accumulator_t diff;

diff = (*a)->rsv - (*b)->rsv;
return diff < 0 ? 1 : diff > 0 ? -1 : 0;
}

/*
	ANT_SEARCH_ENGINE_ACCUMULATOR::COMPARE_POINTER()
	------------------------------------------------
*/
inline int ANT_search_engine_accumulator::compare_pointer(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator *b)
{
ANT_accumulator_t diff;

diff = (*a)->rsv - b->rsv;
return diff < 0 ? 1 : diff > 0 ? -1 : 0;
}


/*
	ANT_SEARCH_ENGINE_ACCUMULATOR::TOP_K_SORT()
	-------------------------------------------
*/
void ANT_search_engine_accumulator::top_k_sort(ANT_search_engine_accumulator **base, long long num, long long top_k)
{
ANT_search_engine_accumulator **lo, **hi;              /* ends of sub-array currently sorting */
ANT_search_engine_accumulator **mid;                  /* points to middle of subarray */
ANT_search_engine_accumulator **loguy, **higuy;        /* traveling pointers for partition step */
size_t size;                /* size of the sub-array */
ANT_search_engine_accumulator **lostk[STKSIZ], **histk[STKSIZ];
int stkptr;                 /* stack for saving sub-array to be processed */

if (num < 2)
	return;                 /* nothing to do */

stkptr = 0;                 /* initialize stack */

lo = base;
hi = base + num - 1;        /* initialize limits */

/* 
	this entry point is for pseudo-recursion calling: setting
	lo and hi and jumping to here is like recursion, but stkptr is
	preserved, locals aren't, so we preserve stuff on the stack 
*/
recurse:

size = (hi - lo) + 1;

if (size <= CUTOFF)
	top_k_shortsort(lo, hi);			// resort to an insertion sort
else
	{
	/* 
		First we pick a partitioning element.  The efficiency of the
		algorithm demands that we find one that is approximately the median
		of the values, but also that we select one fast.  We choose the
		median of the first, middle, and last elements, to avoid bad
		performance in the face of already sorted data, or data that is made
		up of multiple sorted runs appended together.  Testing shows that a
		median-of-three algorithm provides better performance than simply
		picking the middle element for the latter case. 
	*/

	mid = lo + (size / 2);

	/* 
		Sort the first, middle, last elements into order 
	*/
	if (compare_pointer(lo, mid) > 0)
		swap(lo, mid);
	if (compare_pointer(lo, hi) > 0)
		swap(lo, hi);
	if (compare_pointer(mid, hi) > 0)
		swap(mid, hi);

	/* 
		We now wish to partition the array into three pieces, one consisting
		of elements <= partition element, one of elements equal to the
		partition element, and one of elements > than it.  This is done
		below; comments indicate conditions established at every step. 
	*/

	loguy = lo;
	higuy = hi;

	for (;;) 
		{
		do
			loguy++;
		while (loguy <= hi && compare_pointer(loguy, mid) <= 0);

		do
			higuy--;
		while (higuy > mid && compare_pointer(higuy, mid) > 0);

		if (higuy < loguy)
			break;

		swap(loguy, higuy);

		if (mid == higuy)
			mid = loguy;		// If the partition element was moved, follow it
		}

	/*
		Find adjacent elements equal to the partition element.
	*/
	higuy++;
	do
		higuy--;
	while (higuy > lo && compare_pointer(higuy, mid) == 0);
	/* 
		We've finished the partition, now we want to sort the subarrays
		[lo, higuy] and [loguy, hi].
		We do the smaller one first to minimize stack usage.
		We only sort arrays of length 2 or more.
	*/
	if (loguy - lo < top_k)
		if (loguy < hi)
			{
			lostk[stkptr] = loguy;
			histk[stkptr] = hi;
			++stkptr;
			}

	if (lo < higuy)
		{
		hi = higuy;
		goto recurse;
		}
	}

/* 
	We have sorted the array, except for any pending sorts on the stack.
	Check if there are any, and do them.
*/
--stkptr;
if (stkptr >= 0)
	{
	lo = lostk[stkptr];
	hi = histk[stkptr];
	goto recurse;           // pop subarray from stack
	}
}

/*
	ANT_SEARCH_ENGINE_ACCUMULATOR::TOP_K_SHORTSORT()
	------------------------------------------------
	Yes, OK, it doens't do a top-k version, but only called when n is small so that doesn't really matter much (does it?)
*/
void ANT_search_engine_accumulator::top_k_shortsort(ANT_search_engine_accumulator **lo, ANT_search_engine_accumulator **hi)
{
ANT_search_engine_accumulator **p, **max;

while (hi > lo)
	{
	max = lo;
	for (p = lo + 1; p <= hi; p++)
		if (compare_pointer(p, max) > 0)
			max = p;

	swap(max, hi);
	hi--;
	}
}



/*
	Bentley McIlroy Qsort
	---------------------
	Below is a direct copy of the Bentley McIlroy out of their paper and into ANT
	the only difference is the hard-coding for the ANT_search_engine_accumulator
	type.

	For details see:
	J.L. Bentley, M.D. McIlroy (1993), Engineering a Sort Function, Software-Practice and Experience, 23(11):1249-1265
*/

/*
	SEARCH_ENGINE_ACCUMULATOR::BENTLEY_MCILROY_MIN()
	------------------------------------------------
*/
inline long long ANT_search_engine_accumulator::bentley_mcilroy_min(long long a, long long b)
{
return a < b ? a : b;
}

/*
	ANT_SEARCH_ENGINE_ACCUMULATOR::BENTLEY_MCILROY_MED3()
	-----------------------------------------------------
*/
inline ANT_search_engine_accumulator **ANT_search_engine_accumulator::bentley_mcilroy_med3(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b, ANT_search_engine_accumulator **c)
{
return compare_pointer(a, b) < 0 ?
(compare_pointer(b, c) < 0 ? b : compare_pointer(a, c) < 0 ? c : a)
: (compare_pointer(b, c) > 0 ? b : compare_pointer(a, c) > 0 ? c : a);
}

/*
	ANT_SEARCH_ENGINE_ACCUMULATOR::BENTLEY_MCILROY_SWAPFUNC()
	---------------------------------------------------------
*/
inline void ANT_search_engine_accumulator::bentley_mcilroy_swapfunc(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b, long long n)
{
while (n-- > 0)
	swap(a++, b++);
}

/*
	ANT_SEARCH_ENGINE_ACCUMULATOR::BENTLEY_MCILROY_QSORT()
	------------------------------------------------------
*/
void ANT_search_engine_accumulator::bentley_mcilroy_qsort(ANT_search_engine_accumulator **a, long long n, long long top_k)
{
ANT_search_engine_accumulator **pa, **pb, **pc, **pd, **pl, **pm, **pn;
ANT_search_engine_accumulator v, *pv;
long long s;
int r;

if (n < 7) 
	{ /* Insertion sort on smallest arrays */
	for (pm = a + 1; pm < a + n; pm++)
		for (pl = pm; pl > a && compare_pointer(pl - 1, pl) > 0; pl--)
			swap(pl, pl - 1);
	return;
	}
pm = a + (n / 2); /* Small arrays, middle element */
if (n > 7)
	{
	pl = a;
	pn = a + (n - 1);
	if (n > 40) 
		{ /* Big arrays, pseudomedian of 9 */
		s = (n / 8);
		pl = bentley_mcilroy_med3(pl, pl + s, pl + 2 * s);
		pm = bentley_mcilroy_med3(pm - s, pm, pm + s);
		pn = bentley_mcilroy_med3(pn - 2 * s, pn - s, pn);
		}
	pm = bentley_mcilroy_med3(pl, pm, pn); /* Mid-size, med of 3 */
	}

pv = &v;
v = **pm;		/* pv points to partition value */

pa = pb = a;
pc = pd = a + (n - 1);
for (;;)
	{
	while (pb <= pc && (r = compare_pointer(pb, pv)) <= 0)
		{
		if (r == 0)
			{ 
			swap(pa, pb);
			pa++; 
			}
		pb++;
		}
	while (pc >= pb && (r = compare_pointer(pc, pv)) >= 0)
		{
		if (r == 0)
			{
			swap(pc, pd);
			pd--;
			}
		pc--;
		}
	if (pb > pc)
		break;
	swap(pb, pc);
	pb++;
	pc--;
	}
pn = a + n;

s = bentley_mcilroy_min(pa - a, pb - pa);
if (s > 0)
	bentley_mcilroy_swapfunc(a, pb - s, s);

s = bentley_mcilroy_min(pd - pc, pn - pd - 1);
if (s > 0)
	bentley_mcilroy_swapfunc(pb, pn - s, s);

s = pb - pa;
if (s > 1)
	bentley_mcilroy_qsort(a, s, top_k);

s = pd - pc;
if (s > 1)
	bentley_mcilroy_qsort(pn - s, s, top_k);
}
