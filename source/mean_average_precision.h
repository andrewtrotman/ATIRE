
/*
	MEAN_AVERAGE_PRECISION.H
	------------------------
*/

#ifndef MEAN_AVERAGE_PRECISION_H_
#define MEAN_AVERAGE_PRECISION_H_

class ANT_memory;
class ANT_relevant_document;
class ANT_relevant_topic;
class ANT_search_engine;
class ANT_search_engine_accumulator;

class ANT_mean_average_precision
{
private:
	ANT_memory *memory;
	ANT_relevant_document *relevance_list;
	long long relevance_list_length;
	ANT_relevant_topic *topics;
	long long topics_list_length;

	ANT_search_engine_accumulator *accumulators, **results_list;
	long long results_list_length;

private:
	ANT_relevant_topic *setup(long topic, ANT_search_engine *search_engine);

public:
	ANT_mean_average_precision(ANT_memory *memory, ANT_relevant_document *relevance_list, long long relevance_list_length);
	~ANT_mean_average_precision() {}
	
	double average_precision(long topic, ANT_search_engine *search_engine);
	double average_generalised_precision(long topic, ANT_search_engine *search_engine);
	double rank_effectiveness(long topic, ANT_search_engine *search_engine);
	double p_at_n(long topic, ANT_search_engine *search_engine, long precision_point_n);
} ;

#endif  /* MEAN_AVERAGE_PRECISION_H_ */
