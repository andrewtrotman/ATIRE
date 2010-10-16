/*
	STATS.C
	-------
*/
#ifdef _MSC_VER
	#include <stdio.h>
	#include <windows.h>
#elif defined (__APPLE__)
	#include <mach/mach_time.h>
	#include <unistd.h>
	#include <sys/time.h>
	#include <sys/times.h>
#else
	#include <unistd.h>
    #include <sys/time.h>
	#include <sys/times.h>
	#include <sys/param.h> //need predefine HZ
#endif

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

#ifdef PRINT_TIME_NO_CONVERSION
long long ANT_stats::print_time(char *message, long long time_taken, char *end_message) {
	char *units = "milliseconds";
	unsigned long long milliseconds;

	milliseconds = time_to_milliseconds(time_taken);

	printf("%s", message);

	printf("%lld %s%s\n", milliseconds, units, end_message);

	return time_taken;
}
#else
/*
	ANT_STATS::PRINT_TIME()
	-----------------------
*/
long long ANT_stats::print_time(char *message, long long time_taken, char *end_message)
{
char *units = "milliseconds";
long long hours, minutes, seconds, milliseconds;

milliseconds = time_to_milliseconds(time_taken);
seconds = milliseconds / 1000;
minutes = seconds / 60;
hours = minutes / 60;

printf("%s", message);

if (hours > 0)
	printf("%lld:", hours);

if (hours > 0 || minutes > 0)
	printf("%02lld:", minutes % 60);

if (hours > 0 || minutes > 0 || seconds > 0)
	printf("%02lld.", seconds % 60);


if (hours > 0)
	units = "hours";
else if (minutes > 0)
	units = "minutes";
else if (seconds > 0)
	units = "seconds";

printf("%03lld %s%s\n", milliseconds % 1000, units, end_message);

return time_taken;
}
#endif

/*
	ANT_STATS::PRINT_ELAPSED_TIME()
	-------------------------------
*/
long long ANT_stats::print_elapsed_time(long spaced_out)
{
long long total;

total = get_clock_tick() - creation_time;
if (spaced_out)
	print_time("Total Elapsed Time   :", total);
else
	print_time("", total);

return total;
}

/*
	ANT_STATS::GET_CLOCK_TICK()
	---------------------------
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
	struct timeval now;
	gettimeofday(&now, NULL);
	return ((long long)now.tv_sec) * 1000 * 1000 + now.tv_usec;
#endif
}

/*
	ANT_STATS::CLOCK_TICK_FREQUENCY()
	---------------------------------
*/
long long ANT_stats::clock_tick_frequency(void)
{
#ifdef __APPLE__
	mach_timebase_info_data_t info;
	mach_timebase_info(&info);
	return 1000 * 1000 * 1000 * info.numer / info.denom; /* returns in nano seconds */
#elif defined (_MSC_VER)
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return frequency.QuadPart;
#else
	return 1000 * 1000; /* struct coded in microseconds*/
#endif
}

#ifdef _MSC_VER

	/*
		PRINT_FILETIME()
		----------------
	*/
	static void print_filetime(FILETIME *system)
	{
	SYSTEMTIME readable;
	FileTimeToSystemTime(system, &readable);
	printf("%02d:%02d:%02d.%03d", readable.wHour, readable.wMinute, readable.wSecond, readable.wMilliseconds);
	}

#endif


/*
	ANT_STATS::PRINT_OPERATING_SYSTEM_PROCESS_TIME()
	------------------------------------------------
*/
void ANT_stats::print_operating_system_process_time(void)
{
#ifdef _MSC_VER
	FILETIME start, end, kernel, user;

	if (GetProcessTimes(GetCurrentProcess(), &start, &end, &kernel, &user) != 0)
		{
		printf("Windows reports");
		printf(" Kernel:");
		print_filetime(&kernel);
		printf(" User:");
		print_filetime(&user);
		}
#else
	struct tms tmsbuf;
	long clock_speed = sysconf(_SC_CLK_TCK);
	if (times(&tmsbuf) > 0) {
		printf("Kernel: %.3f seconds\n", (double)tmsbuf.tms_stime / clock_speed);
		printf("  User: %.3f seconds\n", (double)tmsbuf.tms_utime / clock_speed);
	}
#endif
}
