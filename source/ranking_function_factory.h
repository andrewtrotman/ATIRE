/*
	RANKING_FUNCTION_FACTORY.H
	--------------------------
*/
#ifndef RANKING_FUNCTION_FACTORY_H_
#define RANKING_FUNCTION_FACTORY_H_

#include "compress.h"
#include "ranking_function_factory_object.h"

class ANT_ranking_function;
extern ANT_ranking_function_factory_object ANT_list_of_rankers[];
class ANT_search_engine;

/*
	class ANT_RANKING_FUNCTION_FACTORY
	----------------------------------
*/
class ANT_ranking_function_factory
{
public:
	ANT_ranking_function_factory() {}
	virtual ~ANT_ranking_function_factory() {}
	
	static ANT_ranking_function *get_indexing_ranker(long long function, long long documents, ANT_compressable_integer *lengths, long long quantization_bits, double p1, double p2, double p3);
	static ANT_ranking_function *get_searching_ranker(ANT_search_engine *search_engine, long long function, long quantization, long long quantization_bits, double p1, double p2, double p3);
} ;

#endif /* RANKING_FUNCTION_FACTORY_H_ */
