/*
	SEARCH_ENGINE_ACCUMULATOR.C
	---------------------------
	Below is a direct copy of the Bentley McIlroy PROGRAM 7 out of their paper and into
	ANT the only differences are the hard-coding for the ANT_search_engine_accumulator
	type and the addition of the top_k parameter

	For details see:
	J.L. Bentley, M.D. McIlroy (1993), Engineering a Sort Function, Software-Practice and Experience, 23(11):1249-1265
*/
#include <stdio.h>
#include <stdlib.h>
#include "search_engine_accumulator.h"

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
	SEARCH_ENGINE_ACCUMULATOR::MIN()
	--------------------------------
*/
inline long long ANT_search_engine_accumulator::min(long long a, long long b)
{
return a < b ? a : b;
}

/*
	ANT_SEARCH_ENGINE_ACCUMULATOR::MED3()
	-------------------------------------
*/
inline ANT_search_engine_accumulator **ANT_search_engine_accumulator::med3(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b, ANT_search_engine_accumulator **c)
{
return compare_pointer(a, b) < 0 ?
	(compare_pointer(b, c) < 0 ? b : compare_pointer(a, c) < 0 ? c : a)
	: (compare_pointer(b, c) > 0 ? b : compare_pointer(a, c) > 0 ? c : a);
}

/*
	ANT_SEARCH_ENGINE_ACCUMULATOR::SWAPFUNC()
	-----------------------------------------
*/
inline void ANT_search_engine_accumulator::swapfunc(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b, long long n)
{
while (n-- > 0)
	swap(a++, b++);
}

/*
	ANT_SEARCH_ENGINE_ACCUMULATOR::TOP_K_SORT()
	-------------------------------------------
*/
void ANT_search_engine_accumulator::top_k_sort(ANT_search_engine_accumulator **a, long long n, long long top_k)
{
ANT_search_engine_accumulator **pa, **pb, **pc, **pd, **pl, **pm, **pn;
ANT_search_engine_accumulator v, *pv;
long long s, s2;
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
		pl = med3(pl, pl + s, pl + 2 * s);
		pm = med3(pm - s, pm, pm + s);
		pn = med3(pn - 2 * s, pn - s, pn);
		}
	pm = med3(pl, pm, pn); /* Mid-size, med of 3 */
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

s = min(pa - a, pb - pa);
if (s > 0)
	swapfunc(a, pb - s, s);

s = min(pd - pc, pn - pd - 1);
if (s > 0)
	swapfunc(pb, pn - s, s);

s = pb - pa;
if (s > 1)
	top_k_sort(a, s, top_k);

if (s < top_k)
	if ((s2 = pd - pc) > 1)
		top_k_sort(pn - s2, s2, top_k - s);
}
