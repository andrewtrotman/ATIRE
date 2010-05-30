/*
	STATS_TIME.H
	------------
*/
#ifndef STATS_TIME_H_
#define STATS_TIME_H_

#include "stats.h"

/*
	class ANT_STATS_TIME
	--------------------
*/
class ANT_stats_time : public ANT_stats
{
public:
	long long disk_input_time;
	long long disk_output_time;
	long long cpu_time;
	long long indexing_time;

public:
	ANT_stats_time(ANT_memory *memory = 0);
	virtual ~ANT_stats_time() {}
	virtual void text_render(void);

	void add(ANT_stats_time *which);

	void add_disk_input_time(long long time) { disk_input_time += time; }
	void add_disk_output_time(long long time) { disk_output_time += time; }
	void add_indexing_time(long long time) { indexing_time += time; }
	void add_cpu_time(long long time) { cpu_time += time; }
} ;



#endif /* STATS_TIME_H_ */
