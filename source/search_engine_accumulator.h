/*
	SEARCH_ENGINE_ACCUMULATOR.H
	---------------------------
*/
#ifndef SEARCH_ENGINE_ACCUMULATOR_H_
#define SEARCH_ENGINE_ACCUMULATOR_H_

#include <stddef.h>		// needed for size_t

#ifndef ANT_ACCUMULATOR_T
	typedef unsigned short ANT_ACCUMULATOR_T;
#endif

#ifdef min
	/* Windows defines the min function, we don't want that. */
#undef min
#endif

/*
	class ANT_SEARCH_ENGINE_ACCUMULATOR
	-----------------------------------
*/
class ANT_search_engine_accumulator
{
friend class ANT_search_engine_result;

public:
	/*
		ANT_accumulator_t is the type used for storing rsv scores in the search engine.  Experiments with 50 topics
		on the TREC WSJ collection suggest that using fixed point arithmetic accurate to 2 decimal places is faster than
		using floating point arithmetic.  The effect on precision is in the 5th decimal place (negligible) and so can be ignored.
		For higher accuracy at an execution cost set ANT_accumulator_t to a double (or float).
	*/
	typedef ANT_ACCUMULATOR_T ANT_accumulator_t;				// short or double (defined in the makefile)

	struct compare_rev
	{
		int operator() (ANT_search_engine_accumulator *a, ANT_search_engine_accumulator *b) const
		{
			return a->rsv < b->rsv ? 1 : a->rsv > b->rsv ? -1 : (a < b ? 1 : a > b ? -1 : 0);
		}
	};

	struct compare
	{
		int operator() (ANT_search_engine_accumulator *a, ANT_search_engine_accumulator *b) const
		{
			return a->rsv > b->rsv ? 1 : a->rsv < b->rsv ? -1 : (a > b ? 1 : a < b ? -1 : 0);
		}
	};

	static int cmp_rsv(const void *a, const void *b)
	{
	ANT_search_engine_accumulator *one, *two;
	one = *(ANT_search_engine_accumulator **)a;
	two = *(ANT_search_engine_accumulator **)b;
	return one->rsv < two->rsv ? -1 : (one->rsv == two->rsv ? 0 : 1);
	}

private:
	ANT_accumulator_t rsv;

	static struct compare_rev cmp;

	static inline void swap(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b) { register ANT_search_engine_accumulator *tmp; tmp = *a; *a = *b; *b = tmp; }

	static inline ANT_search_engine_accumulator **med3(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b, ANT_search_engine_accumulator **c);
	static inline long long min(long long a, long long b);
	static inline void swapfunc(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b, long long n);

protected:
	/* this should be protected and only be called in ANT_search_engine_result class*/
	long is_zero_rsv(void) { return rsv == 0; }

public:
	//ANT_accumulator_t add_rsv(double score) { return rsv += 1 + (ANT_accumulator_t)(score * 100.0); }		// +1 to prevent rounding to zero
	ANT_accumulator_t add_rsv(double score) { return rsv += (ANT_accumulator_t)score; }
	ANT_accumulator_t add_rsv(long score) { return rsv += (ANT_accumulator_t)score; }

	void set_rsv(ANT_accumulator_t score) { rsv = score; }

	ANT_accumulator_t get_rsv(void) { return rsv; }
	void clear_rsv(void) { rsv = 0; }

	static void top_k_sort(ANT_search_engine_accumulator **a, long long n, long long top_k);
} ;

#endif  /* SEARCH_ENGINE_ACCUMULATOR_H_ */
