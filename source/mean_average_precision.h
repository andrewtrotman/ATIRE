
/*
	MEAN_AVERAGE_PRECISION.H
	------------------------
*/

#ifndef __MEAN_AVERAGE_PRECISION_H__
#define __MEAN_AVERAGE_PRECISION_H__

class ANT_memory;
class ANT_relevant_document;
class ANT_relevant_topic;
class ANT_search_engine;

class ANT_mean_average_precision
{
private:
	ANT_memory *memory;
	ANT_relevant_document *relevance_list;
	long long relevance_list_length;
	ANT_relevant_topic *topics;
	long long topics_list_length;

public:
	ANT_mean_average_precision(ANT_memory *memory, ANT_relevant_document *relevance_list, long long relevance_list_length);
	~ANT_mean_average_precision() {}
	
	double average_precision(long topic, ANT_search_engine *search_engine);
	double average_generalised_precision(long topic, ANT_search_engine *search_engine);
	double rank_effectiveness(long topic, ANT_search_engine *search_engine);
} ;

#endif __MEAN_AVERAGE_PRECISION_H__
