/*
	STATS_TIME.C
	------------
*/

#include "stats_time.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_STATS_TIME::ADD_DISK_INPUT_TIME()
	-------------------------------------
*/
ANT_stats_time::ANT_stats_time(ANT_memory *memory) : ANT_stats(memory)
{
indexing_time = disk_input_time = disk_output_time = cpu_time = 0;
}

/*
	ANT_STATS_TIME::ADD()
	---------------------
*/
void ANT_stats_time::add(ANT_stats_time *which)
{
this->disk_input_time += which->disk_input_time;
this->disk_output_time += which->disk_output_time;
this->cpu_time += which->cpu_time;
this->indexing_time += which->indexing_time;
}

/*
	ANT_STATS_TIME::TEXT_RENDER()
	-----------------------------
*/
void ANT_stats_time::text_render(void)
{
long long now;

now = get_clock_tick() - creation_time;
print_time("Disk input time      :", disk_input_time);
print_time("Indexing time        :", indexing_time);
print_time("Disk ouput time      :", disk_output_time);
print_time("Other CPU time       :", now - disk_input_time - disk_output_time - indexing_time);
print_time("Total CPU time       :", now - disk_input_time - disk_output_time);
print_elapsed_time(TRUE);
}

