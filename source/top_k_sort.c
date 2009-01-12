/*
	TOP_K_SORT.C
	------------
	Based on the qsort implementation in the C Runtime Library shipped with Microsoft Visual Studio 2008
*/
#include <stdlib.h>
#include "top_k_sort.h"
//#include "search_engine_accumulator.h"

static void top_k_shortsort (char *lo, char *hi, size_t width, int (*comp)(const void *, const void *));

/*
	this parameter defines the cutoff between using quick sort and
	insertion sort for arrays; arrays with lengths shorter or equal to the
	below value use insertion sort 
*/
#define CUTOFF 8            /* testing shows that this is good value */

/*
	Note: the theoretical number of stack entries required is
	no more than 1 + log2(num).  But we switch to insertion
	sort for CUTOFF elements or less, so we really only need
	1 + log2(num) - log2(CUTOFF) stack entries.  For a CUTOFF
	of 8, that means we need no more than 30 stack entries for
	32 bit platforms, and 62 for 64-bit platforms. 
*/
#define STKSIZ (8 * sizeof(void *) - 2)

#define __COMPARE(p1, p2) comp(p1, p2)
#define __SHORTSORT(lo, hi, width, comp) top_k_shortsort(lo, hi, width, comp)


/*
	SWAP ()
	-------
*/
static inline void swap (char *a, char *b, size_t width)
{
char tmp;

if ( a != b )
	while (width--)
		{
		tmp = *a;
		*a++ = *b;
		*b++ = tmp;
		}
}

/*
	TOP_K_SORT()
	------------
*/
void top_k_sort(void *base, size_t num, size_t width, int (*comp)(const void *, const void *))
{
char *lo, *hi;              /* ends of sub-array currently sorting */
char *mid;                  /* points to middle of subarray */
char *loguy, *higuy;        /* traveling pointers for partition step */
size_t size;                /* size of the sub-array */
char *lostk[STKSIZ], *histk[STKSIZ];
int stkptr;                 /* stack for saving sub-array to be processed */

if (num < 2)
	return;                 /* nothing to do */

stkptr = 0;                 /* initialize stack */

lo = (char *)base;
hi = (char *)base + width * (num - 1);        /* initialize limits */

/* 
	this entry point is for pseudo-recursion calling: setting
	lo and hi and jumping to here is like recursion, but stkptr is
	preserved, locals aren't, so we preserve stuff on the stack 
*/
recurse:

size = (hi - lo) / width + 1;        /* number of el's to sort */

    /* below a certain size, it is faster to use a O(n^2) sorting method */
if (size <= CUTOFF)
	__SHORTSORT(lo, hi, width, comp);
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

	mid = lo + (size / 2) * width;      /* find middle element */

	/* Sort the first, middle, last elements into order */
	if (__COMPARE(lo, mid) > 0)
		swap(lo, mid, width);
	if (__COMPARE(lo, hi) > 0)
		swap(lo, hi, width);
	if (__COMPARE(mid, hi) > 0)
		swap(mid, hi, width);

	/* 
		We now wish to partition the array into three pieces, one consisting
		of elements <= partition element, one of elements equal to the
		partition element, and one of elements > than it.  This is done
		below; comments indicate conditions established at every step. 
	*/

	loguy = lo;
	higuy = hi;

	/* Note that higuy decreases and loguy increases on every iteration,
	so loop must terminate. */
	for (;;) 
		{
		do
			loguy += width;
		while (loguy <= hi && __COMPARE(loguy, mid) <= 0);

		do
			higuy -= width;
		while (higuy > mid && __COMPARE(higuy, mid) > 0);

		if (higuy < loguy)
			break;

		swap(loguy, higuy, width);

		/* If the partition element was moved, follow it. */

		if (mid == higuy)
			mid = loguy;
		}

	/*
		Find adjacent elements equal to the partition element.
	*/
	higuy += width;
	do
		higuy -= width;
	while (higuy > lo && __COMPARE(higuy, mid) == 0);
	/* 
		We've finished the partition, now we want to sort the subarrays
		[lo, higuy] and [loguy, hi].
		We do the smaller one first to minimize stack usage.
		We only sort arrays of length 2 or more.
	*/

	if ( higuy - lo >= hi - loguy )
		{
		if (lo < higuy) 
			{
			lostk[stkptr] = lo;
			histk[stkptr] = higuy;
			++stkptr;
			}                           /* save big recursion for later */

		if (loguy < hi) 
			{
			lo = loguy;
			goto recurse;           /* do small recursion */
			}
		}
	else
		{
		if (loguy < hi)
			{
			lostk[stkptr] = loguy;
			histk[stkptr] = hi;
			++stkptr;               /* save big recursion for later */
			}

		if (lo < higuy)
			{
			hi = higuy;
			goto recurse;           /* do small recursion */
			}
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
	goto recurse;           /* pop subarray from stack */
	}
else
	return;                 /* all subarrays done */
}

/*
	TOP_K_SHORTSORT ()
	------------------
*/
static void top_k_shortsort (char *lo, char *hi, size_t width, int (*comp)(const void *, const void *))
{
char *p, *max;

while (hi > lo)
	{
	max = lo;
	for (p = lo + width; p <= hi; p += width)
		if (__COMPARE(p, max) > 0)
			max = p;

	swap(max, hi, width);
	hi -= width;
	}
}


