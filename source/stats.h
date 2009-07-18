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

public:
	ANT_stats(ANT_memory *memory = 0);
	virtual ~ANT_stats();

	long long print_elapsed_time(long spaced_out = 0);
	long long print_time(char *message, long long time_taken, char *end_message = "");

	long long get_clock_tick_frequency(void) { return tick_frequency; }

	static long long clock_tick_frequency(void);
	static long long get_clock_tick(void);

	static long long start_timer(void) { return get_clock_tick(); }
	static long long stop_timer(long long timer) { return get_clock_tick() - timer; }

	static void print_operating_system_process_time(void);

	virtual void text_render(void) {}
} ;

#endif __STATS_H__
