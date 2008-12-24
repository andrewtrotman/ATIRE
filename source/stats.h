/*
	STATS.H
	-------
*/

#ifndef __STATS_H__
#define __STATS_H__

class ANT_memory;

class ANT_stats
{
public:
	ANT_memory *memory;
	long long tick_frequency;
	long long creation_time;
private:
	long long clock_tick_frequency(void);

public:
	ANT_stats(ANT_memory *memory = 0);
	virtual ~ANT_stats();

	long long print_elapsed_time(void);
	long long print_time(char *message, long long time_taken, char *end_message = "");

	virtual void text_render(void) = 0;
	long long get_clock_tick(void);
	long long get_clock_tick_frequency(void) { return tick_frequency; }

	long long start_timer(void) { return get_clock_tick(); }
	long long stop_timer(long long timer) { return get_clock_tick() - timer; }
} ;

#endif __STATS_H__
