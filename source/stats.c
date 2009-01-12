/*
	STATS.C
	-------
*/
#include <windows.h>
#include <stdio.h>
#include "stats.h"

/*
	ANT_STATS::ANT_STATS()
	----------------------
*/
ANT_stats::ANT_stats(ANT_memory *memory)
{
this->memory = memory;
creation_time = get_clock_tick();
tick_frequency = clock_tick_frequency();
}

/*
	ANT_STATS::~ANT_STATS()
	-----------------------
*/
ANT_stats::~ANT_stats()
{
}

/*
	ANT_STATS::PRINT_TIME()
	-----------------------
*/
long long ANT_stats::print_time(char *message, long long time_taken, char *end_message)
{
char *units = "milliseconds";
long long hours, minutes, seconds, milliseconds;

milliseconds = (long long)(time_taken / (get_clock_tick_frequency() / 1000.0));
seconds = milliseconds / 1000;
minutes = seconds / 60;
hours = minutes / 60;

printf("%s", message);

if (hours > 0)
	{
	printf("%lld:", hours);
	units = "";
	}

if (hours > 0 || minutes > 0)
	{
	printf("%02lld:", minutes % 60);
	units = "minutes";
	}

if (hours > 0 || minutes > 0 || seconds > 0)
	{
	printf("%02lld.", seconds % 60);
	units = "seconds";
	}

printf("%03lld %s%s\n", milliseconds % 1000, units, end_message);

return time_taken;
}

/*
	ANT_STATS::PRINT_ELAPSED_TIME()
	-------------------------------
*/
long long ANT_stats::print_elapsed_time(void)
{
long long total;

total = get_clock_tick() - creation_time;
print_time("Total Elapsed Time:", total);

return total;
}

/*
	LONG ANT_STATS::GET_CLOCK_TICK()
	--------------------------------
*/
long long ANT_stats::get_clock_tick(void)
{
#ifdef __APPLE__
	uint64_t now;
	now = mach_absolute_time();
	return (long long)now;
#elif defined (_MSC_VER)
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return now.QuadPart;
#else
	#error "Cannot determine OS for timing code"
#endif
}

/*
	LONG ANT_STATS::CLOCK_TICK_FREQUENCY()
	--------------------------------------
*/
long long ANT_stats::clock_tick_frequency(void)
{
#ifdef __APPLE__
	return AbsoluteToNanoseconds(1);
#elif defined (_MSC_VER)
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return frequency.QuadPart;
#else
	#error "Cannot determine OS for timing code"
#endif
}

