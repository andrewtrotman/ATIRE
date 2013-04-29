/*
	RANKING_FUNCTION_FACTORY.H
	--------------------------
*/
#ifndef RANKING_FUNCTION_FACTORY_H_
#define RANKING_FUNCTION_FACTORY_H_

#include "compress.h"

class ANT_ranking_function;

/*
	class ANT_RANKING_FUNCTION_FACTORY
	----------------------------------
*/
class ANT_ranking_function_factory
{
public:
	ANT_ranking_function_factory() {}
	virtual ~ANT_ranking_function_factory() {}
	
	virtual ANT_ranking_function *get_indexing_ranker(long long documents, ANT_compressable_integer *lengths, long *index_quantization, long long *quantization_bits) = 0;
} ;

#endif /* RANKING_FUNCTION_FACTORY_H_ */
