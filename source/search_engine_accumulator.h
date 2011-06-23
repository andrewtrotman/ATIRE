/*
	SEARCH_ENGINE_ACCUMULATOR.H
	---------------------------
*/
#ifndef SEARCH_ENGINE_ACCUMULATOR_H_
#define SEARCH_ENGINE_ACCUMULATOR_H_

#include <stddef.h>		// needed for size_t

#ifndef ANT_ACCUMULATOR_T
	typedef short ANT_ACCUMULATOR_T;
#endif

/*
	class ANT_SEARCH_ENGINE_ACCUMULATOR
	-----------------------------------
*/
class ANT_search_engine_accumulator
{
public:
	/*
		ANT_accumulator_t is the type used for storing rsv scores in the search engine.  Experiments with 50 topics
		on the TREC WSJ collection suggest that using fixed point arithmetic accurate to 2 decimal places is faster than
		using floating point arithmetic.  The effect on precision is in the 5th decimal place (negligible) and so can be ignored.
		For higher accuracy at an execution cost set ANT_accumulator_t to a double (or float).
	*/
	typedef ANT_ACCUMULATOR_T ANT_accumulator_t;				// short or double (defined in the makefile)

	enum { SORT_NONE, SORT_RSV, SORT_ID_ASC, SORT_ID_DESC };	// how accumulators should be sorted

private:
	ANT_accumulator_t rsv;

private:
	static inline void swap(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b) { register ANT_search_engine_accumulator *tmp; tmp = *a; *a = *b; *b = tmp; }
	static inline int compare_pointer(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b);

	static inline int compare_accumulator_position(ANT_search_engine_accumulator *a, ANT_search_engine_accumulator *b);


	/*
		ANT_SEARCH_ENGINE_ACCUMULATOR::SWAPFUNC()
		-----------------------------------------
	*/
	static inline void swapfunc(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b, long long n)
	{
	while (n-- > 0)
		swap(a++, b++);
	}

	struct compare_rsv {
		int operator()(ANT_search_engine_accumulator *a, ANT_search_engine_accumulator *b) const
		{
		ANT_accumulator_t diff;

		diff = a->rsv - b->rsv;
		return diff < 0 ? 1 : diff > 0 ? -1 : 0;
		}
	};

	struct compare_id_asc {
		int operator()(ANT_search_engine_accumulator *a, ANT_search_engine_accumulator *b) const
		{
		return a < b ? -1 : a > b ? 1 : 0;
		}
	};

	struct compare_id_desc {
		int operator()(ANT_search_engine_accumulator *a, ANT_search_engine_accumulator *b) const
		{
		return a < b ? 1 : a > b ? -1 : 0;
		}
	};

	/*
		SEARCH_ENGINE_ACCUMULATOR::MIN()
		--------------------------------
	*/
	static inline long long min(long long a, long long b)
	{
	return a < b ? a : b;
	}

	/*
		ANT_SEARCH_ENGINE_ACCUMULATOR::MED3()
		-------------------------------------
	*/
	template <typename _Compare>
	static inline ANT_search_engine_accumulator **med3(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b, ANT_search_engine_accumulator **c, const _Compare & compare)
	{
	return compare(*a, *b) < 0 ?
		(compare(*b, *c) < 0 ? b : compare(*a, *c) < 0 ? c : a)
		: (compare(*b, *c) > 0 ? b : compare(*a, *c) > 0 ? c : a);
	}


	/*
		ANT_SEARCH_ENGINE_ACCUMULATOR::TOP_K_SORT_IMPL()
		-------------------------------------------
		Below is a direct copy of the Bentley McIlroy PROGRAM 7 out of their paper and into
		ANT the only differences are the hard-coding for the ANT_search_engine_accumulator
		type and the addition of the top_k parameter

		For details see:
		J.L. Bentley, M.D. McIlroy (1993), Engineering a Sort Function, Software-Practice and Experience, 23(11):1249-1265
	*/
	template <typename _Compare>
	static void top_k_sort_impl(ANT_search_engine_accumulator **a, long long n, long long top_k, const _Compare & compare)
	{
	ANT_search_engine_accumulator **pa, **pb, **pc, **pd, **pl, **pm, **pn;
	ANT_search_engine_accumulator *pv;
	long long s, s2;
	int r;

	if (n < 7)
		{ /* Insertion sort on smallest arrays */
		for (pm = a + 1; pm < a + n; pm++)
			for (pl = pm; pl > a && compare(*(pl - 1), *pl) > 0; pl--)
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
			pl = med3(pl, pl + s, pl + 2 * s, compare);
			pm = med3(pm - s, pm, pm + s, compare);
			pn = med3(pn - 2 * s, pn - s, pn, compare);
			}
		pm = med3(pl, pm, pn, compare); /* Mid-size, med of 3 */
		}

	pv = *pm;		/* pv points to partition value */

	pa = pb = a;
	pc = pd = a + (n - 1);
	for (;;)
		{
		while (pb <= pc && (r = compare(*pb, pv)) <= 0)
			{
			if (r == 0)
				{
				swap(pa, pb);
				pa++;
				}
			pb++;
			}
		while (pc >= pb && (r = compare(*pc, pv)) >= 0)
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
		top_k_sort_impl(a, s, top_k, compare);

	if (s < top_k)
		if ((s2 = pd - pc) > 1)
			top_k_sort_impl(pn - s2, s2, top_k - s, compare);
	}

public:
	ANT_accumulator_t add_rsv(double score) { return rsv += 1 + (ANT_accumulator_t)(score * 100.0); }		// +1 to prevent rounding to zero
	ANT_accumulator_t add_rsv(long score) { return rsv += (ANT_accumulator_t)score; }

	long is_zero_rsv(void) { return rsv == 0; }
	ANT_accumulator_t get_rsv(void) { return rsv; }
	void clear_rsv(void) { rsv = 0; }

	static inline void top_k_sort(ANT_search_engine_accumulator **a, long long n, long long top_k, int sort_order = SORT_RSV)
	{
	switch (sort_order)
		{
		case SORT_ID_ASC:
			top_k_sort_impl(a, n, top_k, compare_id_asc());
			break;
		case SORT_ID_DESC:
			top_k_sort_impl(a, n, top_k, compare_id_desc());
			break;

		case SORT_RSV:
		default:
			top_k_sort_impl(a, n, top_k, compare_rsv());
		}
	}
} ;

#endif  /* SEARCH_ENGINE_ACCUMULATOR_H_ */
