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
class ANT_focus_results_list;

/*
	class ANT_MEAN_AVERAGE_PRECISION
	--------------------------------
*/
class ANT_mean_average_precision
{
private:
	ANT_memory *memory;
	ANT_relevant_document *relevance_list;
	long long relevance_list_length;
	ANT_relevant_topic *topics;
	long long topics_list_length;

private:
	ANT_relevant_topic *setup(long topic);

public:
	ANT_mean_average_precision(ANT_memory *memory, ANT_relevant_document *relevance_list, long long relevance_list_length);
	~ANT_mean_average_precision() {}
	
	double average_precision(long topic, ANT_search_engine *search_engine);
	double average_interpolated_precision(long topic, ANT_focus_results_list *results_list);
	double average_generalised_precision_document(long topic, ANT_search_engine *search_engine);
	long long MAgP_crossover(long long start, long long finish, long long relevant_start, long long relevant_finish);
	double average_generalised_precision_focused(long topic, ANT_focus_results_list *results_list);
	double rank_effectiveness(long topic, ANT_search_engine *search_engine);
	double bpref(long topic, ANT_search_engine *search_engine);
	double p_at_n(long topic, ANT_search_engine *search_engine, long precision_point_n);
	double success_at_n(long topic, ANT_search_engine *search_engine, long precision_point_n);
	double ndcg(long topic, ANT_search_engine *search_engine, long precision_point_n);
	double ndcgt(long topic, ANT_search_engine *search_engine, long precision_point_n);
	double err(long topic, ANT_search_engine *search_engine);

	static int gain_compare(const void *a, const void *b);
} ;

#endif  /* MEAN_AVERAGE_PRECISION_H_ */
