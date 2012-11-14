/*
	STATS_SEARCH_ENGINE.C
	---------------------
*/

#include <stdio.h>
#include "stats_search_engine.h"

/*
	ANT_STATS_SEARCH_ENGINE::ANT_STATS_SEARCH_ENGINE()
	--------------------------------------------------
*/
ANT_stats_search_engine::ANT_stats_search_engine(ANT_memory *memory) : ANT_stats_time(memory)
{
initialise();
}

/*
	ANT_STATS_SEARCH_ENGINE::INITIALISE()
	-------------------------------------
*/
void ANT_stats_search_engine::initialise(void)
{
thesaurus_reencode_time = thesaurus_time = stemming_reencode_time = stemming_time = dictionary_time = count_relevant_time = 0;
sort_time = accumulator_init_time = posting_read_time = decompress_time = rank_time = 0;
disk_bytes_read_on_init = disk_bytes_read_on_search = 0;
queries = 0;
}

/*
	ANT_STATS_SEARCH_ENGINE::ADD()
	------------------------------
*/
void ANT_stats_search_engine::add(ANT_stats_search_engine *which)
{
ANT_stats_time::add(which);
this->decompress_time += which->decompress_time;
this->posting_read_time += which->posting_read_time;
this->rank_time += which->rank_time;
this->accumulator_init_time += which->accumulator_init_time;
this->sort_time += which->sort_time;
this->count_relevant_time += which->count_relevant_time;
this->dictionary_time += which->dictionary_time;
this->stemming_time += which->stemming_time;
this->stemming_reencode_time += which->stemming_reencode_time;
this->thesaurus_time += which->thesaurus_time;
this->thesaurus_reencode_time += which->thesaurus_reencode_time;
this->queries++;
this->disk_bytes_read_on_init += which->disk_bytes_read_on_init;
this->disk_bytes_read_on_search += which->disk_bytes_read_on_search;
}

/*
	ANT_STATS_SEARCH_ENGINE::TEXT_RENDER()
	--------------------------------------
*/
void ANT_stats_search_engine::text_render(void)
{
long long min, sum;

if (disk_bytes_read_on_init != 0)
	printf("Disk Bytes Read Init  :%lld bytes\n", disk_bytes_read_on_init);
printf("Disk Bytes Read Search:%lld bytes\n", disk_bytes_read_on_search);
print_time("Accumulator Init Time :", accumulator_init_time);
print_time("Dictionary Read Time  :", dictionary_time);
print_time("Posting Disk Read Time:", posting_read_time);
if (stemming_time != 0)
	{
	print_time("Stem Computation Time :", stemming_time);
	print_time("Stem to Postings Time :", stemming_reencode_time);
	}
if (thesaurus_time != 0)
	{
	print_time("Thes Computation Time :", thesaurus_time);
	print_time("Thes to Postings Time :", thesaurus_reencode_time);
	}
print_time("Decompress Time       :", decompress_time);
print_time("Rank Time             :", rank_time);
print_time("Sort Time             :", sort_time);
print_time("Count Relevant Time   :", count_relevant_time);

sum = 0;
sum += accumulator_init_time;
sum += dictionary_time;
sum += posting_read_time;
sum += decompress_time;
sum += rank_time;
sum += sort_time;
sum += count_relevant_time;
sum += stemming_time;
//sum += stemming_reencode_time;		/* Don't add the re-encode time as it is already part of the rank_time */

print_time("Total Time to Search  :", sum);
if (queries > 1)
	{
	printf("Total Queries         :%lld\n", queries);
	print_time("Mean Time to Search   :", sum / queries);
	}

min = posting_read_time;
min = min < decompress_time ? min : decompress_time;
min = min < rank_time ? min : rank_time;

printf("Ratio (read/decompress/rank):%2.2f %2.2f %2.2f\n", posting_read_time / (double)min, decompress_time / (double)min, rank_time / (double)min);
}
