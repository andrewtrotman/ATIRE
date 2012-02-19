/*
	STATS_MEMORY_INDEX.C
	--------------------
*/
#include <stdio.h>
#include "memory.h"
#include "memory_index.h"
#include "stats_memory_index.h"
#include "postings_piece.h"

/*
	ANT_STATS_MEMORY_INDEX::ANT_STATS_MEMORY_INDEX()
	------------------------------------------------
*/
ANT_stats_memory_index::ANT_stats_memory_index(ANT_memory *memory, ANT_memory *postings_memory) : ANT_stats(memory)
{
this->postings_memory = postings_memory;
hash_nodes = 0;
unique_terms = 0;
documents = 0;
term_occurences = 0;
posting_fragments = 0;
bytes_to_store_docids = 0;
bytes_allocated_for_docids = 0;
bytes_to_store_tfs = 0;
bytes_allocated_for_tfs = 0;
strings = 0;
bytes_in_string_pool = 0;
disk_buffer = 0;
bytes_used_to_sort_term_list = 0;
bytes_for_decompression_recompression = 0;
time_to_quantize = 0;
bytes_to_quantize = 0;
time_to_store_documents_on_disk = 0;
bytes_to_store_documents_on_disk = 0;
}

/*
	ANT_STATS_MEMORY_INDEX::~ANT_STATS_MEMORY_INDEX()
	-------------------------------------------------
*/
ANT_stats_memory_index::~ANT_stats_memory_index()
{
}

/*
	ANT_STATS_MEMORY_INDEX::TEXT_RENDER()
	-------------------------------------
*/
void ANT_stats_memory_index::text_render(long type)
{
long long used, sum;

if (type & STAT_SUMMARY)
	{
	printf("GLOBAL STATISTICS\n-----------------\n");
	printf("Documents            :%10lld documents\n", documents);
	printf("Unique terms         :%10ld terms\n", unique_terms);
	printf("Term occurences      :%10lld occurences\n", term_occurences);
	if (memory != NULL)
		{
		printf("Total memory allocd  :%10lld bytes\n", memory->bytes_allocated() + postings_memory->bytes_allocated());
		printf("Total memory used    :%10lld bytes\n", memory->bytes_used() + postings_memory->bytes_used());
		}
	}

if (type & STAT_MEMORY)
	{
	printf("\nINTERNAL MEMORY BREAKDOWN\n-------------------------\n");
	printf("Hash nodes total     :%10d nodes\n", (int)ANT_memory_index::HASH_TABLE_SIZE);
	printf("Hash nodes used      :%10ld nodes\n", hash_nodes);
	printf("Hash nodes used      :%10ld bytes\n", (long)(hash_nodes * sizeof(ANT_memory_index_hash_node)));
	printf("HASH utilisation     :%10.2f%%\n", ((double)hash_nodes / (double)ANT_memory_index::HASH_TABLE_SIZE) * 100);

	printf("Mem used for docIDs  :%10lld bytes\n", bytes_to_store_docids);
	printf("Mem allocd for docIDs:%10lld bytes\n", bytes_allocated_for_docids);
	printf("DOCID mem utilisation:%10.2f%%\n", ((double)bytes_to_store_docids / (double)bytes_allocated_for_docids) * 100);

	printf("Mem used for tfs     :%10lld bytes\n", bytes_to_store_tfs);
	printf("Mem allocd for tfs   :%10lld bytes\n", bytes_allocated_for_tfs);
	printf("TF mem utilisation   :%10.2f%%\n", ((double)bytes_to_store_tfs / (double)bytes_allocated_for_tfs) * 100);

	printf("Postings fragments   :%10lld fragments\n", posting_fragments);
	printf("Postings fragments   :%10lld bytes\n", posting_fragments * sizeof(ANT_postings_piece));

	printf("Total allocd for postings (fragments + docIDs + tfs):%10lld bytes\n", sum = posting_fragments * sizeof(ANT_postings_piece) + bytes_allocated_for_docids + bytes_allocated_for_tfs);
	printf("Total needed for postings (docIDs + tfs)            :%10lld\n", used = bytes_to_store_docids + bytes_to_store_tfs);
	printf("Total wasted for postings                           :%10lld\n", sum - used);
	printf("Total Postings utilisation                          :%10.2f%%\n", ((double)used / (double)sum) * 100);

	printf("Total strings        :%10lld strings\n", strings);
	printf("Space for strings    :%10lld bytes\n", bytes_in_string_pool);
	printf("Disk buffering       :%10ld bytes\n", disk_buffer);
	printf("Recompression Buffers:%10lld bytes\n", bytes_for_decompression_recompression);

	if (memory != NULL)
		{
		printf("\nMEMORY UTILISATION\n------------------\n");
		printf("Total memory allocd  :%10lld bytes\n", memory->bytes_allocated() + postings_memory->bytes_allocated());
		printf("Total memory used    :%10lld bytes\n", memory->bytes_used() + postings_memory->bytes_used());
		sum = 0;
		printf("Posting Fragments    :%10lld bytes\n", used = posting_fragments * sizeof(ANT_postings_piece));
		sum += used;
		printf("Unique Terms         :%10lld bytes\n", used = unique_terms * sizeof(ANT_memory_index_hash_node));
		sum += used;
		printf("Strings              :%10lld bytes\n", used = bytes_in_string_pool);
		sum += used;
		printf("DocIDs               :%10lld bytes\n", used = bytes_allocated_for_docids);
		sum += used;
		printf("TFs                  :%10lld bytes\n", used = bytes_allocated_for_tfs);
		sum += used;
		printf("Term List (uniq sort):%10lld bytes\n", used = bytes_used_to_sort_term_list);
		sum += used;
		printf("Disk Output Buffer   :%10lld bytes\n", used = disk_buffer);
		sum += used;
		printf("Recompression Buffers:%10lld bytes\n", used = bytes_for_decompression_recompression);
		sum += used;
		printf("Quantization overhead:%10lld bytes\n", used = bytes_to_quantize);
		sum += used;
		printf("Unaccounted for      :%10lld bytes\n", memory->bytes_used() + postings_memory->bytes_used() - sum);
		}
	}

if (type & STAT_SUMMARY)
	if (time_to_quantize != 0)
		print_time("Time to Quantise     :", time_to_quantize);

if (time_to_store_documents_on_disk != 0)
	{
	print_time("Time to Store Docs   :", time_to_store_documents_on_disk);
	printf("Bytes to Store Docs  :%10lld bytes\n", bytes_to_store_documents_on_disk);
	}
}
