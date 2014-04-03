/*
	RANKING_FUNCTION_QUANTUM_PARAMETERS.H
	-------------------------------------
*/
#ifndef __RANKING_FUNCTION_QUANTUM_PARAMETERS_H__
#define __RANKING_FUNCTION_QUANTUM_PARAMETERS_H__

#include "search_engine_result.h"
#include "impact_header.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"

class ANT_ranking_function_quantum_parameters
{
public:
	ANT_search_engine_result *accumulator;
	ANT_search_engine_btree_leaf *term_details;
	double tf;
	ANT_compressable_integer *the_quantum;
	ANT_compressable_integer *quantum_end;
	long long trim_point;
	double prescalar;
	double postscalar;
	double query_frequency;
};

#endif  /* __RANKING_FUNCTION_QUANTUM_PARAMETERS_H__ */
