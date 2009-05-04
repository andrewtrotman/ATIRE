
/*
	TIME_STATS.C
	------------
*/
#include "time_stats.h"

/*
	ANT_TIME_STATS::ADD_DISK_INPUT_TIME()
	-------------------------------------
*/
ANT_time_stats::ANT_time_stats(ANT_memory *memory) : ANT_stats(memory)
{
disk_input_time = disk_output_time = cpu_time = 0;
}

/*
	ANT_TIME_STATS::TEXT_RENDER()
	-----------------------------
*/
void ANT_time_stats::text_render(void)
{
long long now;

now = get_clock_tick() - creation_time;
print_time("Disk input time      :", disk_input_time);
print_time("Disk ouput time      :", disk_output_time);
print_time("CPU time             :", now - disk_input_time - disk_output_time);
print_elapsed_time();
}

