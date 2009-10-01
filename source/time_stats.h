/*
	TIME_STATS.H
	------------
*/

#ifndef TIME_STATS_H_
#define TIME_STATS_H_

#include "stats.h"

class ANT_time_stats : public ANT_stats
{
public:
	long long disk_input_time;
	long long disk_output_time;
	long long cpu_time;

public:
	ANT_time_stats(ANT_memory *memory = 0);
	virtual ~ANT_time_stats() {}
	virtual void text_render(void);

	void add(ANT_time_stats *which);

	void add_disk_input_time(long long time) { disk_input_time += time; }
	void add_disk_output_time(long long time) { disk_output_time += time; }
	void add_cpu_time(long long time) { cpu_time += time; }
} ;

#endif  /* TIME_STATS_H_ */
