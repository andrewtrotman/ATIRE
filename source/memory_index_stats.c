/*
	MEMORY_INDEX_STATS.C
	--------------------
*/
#include <stdio.h>
#include "memory.h"
#include "memory_index.h"
#include "memory_index_stats.h"
#include "postings_piece.h"

/*
	ANT_MEMORY_INDEX_STATS::ANT_MEMORY_INDEX_STATS()
	------------------------------------------------
*/
ANT_memory_index_stats::ANT_memory_index_stats(ANT_memory *memory)
{
this->memory = memory;
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
}

/*
	ANT_MEMORY_INDEX_STATS::~ANT_MEMORY_INDEX_STATS()
	-------------------------------------------------
*/
ANT_memory_index_stats::~ANT_memory_index_stats()
{
}

/*
	ANT_MEMORY_INDEX_STATS::TEXT_RENDER()
	-------------------------------------
*/
void ANT_memory_index_stats::text_render(void)
{
long long used, sum;

printf("GLOBAL STATISTICS\n-----------------\n");
printf("Documents            :%10I64d documents\n", documents);
printf("Unique terms         :%10d terms\n", unique_terms);
printf("Term occurences      :%10I64d occurences\n", term_occurences);

printf("\nINTERNAL MEMORY BREAKDOWN\n-------------------------\n");
printf("Hash nodes total     :%10d nodes\n", HASH_TABLE_SIZE);
printf("Hash nodes used      :%10d nodes\n", hash_nodes);
printf("HASH utilisation     :%10.2f%%\n", ((double)hash_nodes / (double)HASH_TABLE_SIZE) * 100);
printf("Mem used for docIDs  :%10I64d bytes\n", bytes_to_store_docids);
printf("Mem allocd for docIDs:%10I64d bytes\n", bytes_allocated_for_docids);
printf("DOCID mem utilisation:%10.2f%%\n", ((double)bytes_to_store_docids / (double)bytes_allocated_for_docids) * 100);
printf("Mem used for tfs     :%10I64d bytes\n", bytes_to_store_tfs);
printf("Mem allocd for tfs   :%10I64d bytes\n", bytes_allocated_for_tfs);
printf("TF mem utilisation   :%10.2f%%\n", ((double)bytes_to_store_tfs / (double)bytes_allocated_for_tfs) * 100);
printf("Postings fragments   :%10I64d fragments\n", posting_fragments);
printf("Total strings        :%10I64d strings\n", strings);
printf("Space for strings    :%10I64d bytes\n", bytes_in_string_pool);
printf("Disk buffering       :%10d bytes\n", disk_buffer);

printf("\nMEMORY UTILISATION\n------------------\n");
printf("Total memory allocd  :%10I64d bytes\n", memory->bytes_allocated());
printf("Total memory used    :%10I64d bytes\n", memory->bytes_used());
sum = 0;
printf("Posting Fragments    :%10I64d bytes\n", used = posting_fragments * sizeof(ANT_postings_piece));
sum += used;
printf("Unique Terms         :%10I64d bytes\n", used = unique_terms * sizeof(ANT_memory_index_hash_node));
sum += used;
printf("Strings              :%10I64d bytes\n", used = bytes_in_string_pool);
sum += used;
printf("DocIDs               :%10I64d bytes\n", used = bytes_allocated_for_docids);
sum += used;
printf("TFs                  :%10I64d bytes\n", used = bytes_allocated_for_tfs);
sum += used;
printf("Disk Output Buffer   :%10I64d bytes\n", used = disk_buffer);
sum += used;
printf("Unaccounted for      :%10I64d bytes\n", memory->bytes_used() - sum);
}

