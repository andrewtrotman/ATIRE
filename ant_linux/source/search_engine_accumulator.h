/*
	SEARCH_ENGINE_ACCUMULATORS.H
	----------------------------
*/

#ifndef __SEARCH_ENGINE_ACCUMULATORS_H__
#define __SEARCH_ENGINE_ACCUMULATORS_H__

class ANT_search_engine_accumulator
{
public:
	long docid;
	double rsv;

public:
	static int compare(const void *a, const void *b);
} ;

#endif /* __SEARCH_ENGINE_ACCUMULATORS_H__ */
