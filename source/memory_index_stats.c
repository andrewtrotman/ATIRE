/*
	MEMORY_INDEX_STATS.C
	--------------------
*/
#include <stdio.h>
#include "memory_index.h"
#include "memory_index_stats.h"

/*
	ANT_MEMORY_INDEX_STATS::ANT_MEMORY_INDEX_STATS()
	------------------------------------------------
*/
ANT_memory_index_stats::ANT_memory_index_stats()
{
hash_nodes = 0;
unique_terms = 0;
documents = 0;
term_occurences = 0;
bytes_to_store_docids = 0;
bytes_allocated_for_docids = 0;
}

/*
	ANT_MEMORY_INDEX_STATS::~ANT_MEMORY_INDEX_STATS()
	-------------------------------------------------
*/
ANT_memory_index_stats::~ANT_memory_index_stats()
{
}

/*
	ANT_MEMORY_INDEX_STATS::RENDER()
	--------------------------------
*/
void ANT_memory_index_stats::render(void)
{
printf("Documents            :%I64d\n", documents);
printf("Hash nodes total     :%d\n", HASH_TABLE_SIZE);
printf("Hash nodes used      :%d\n", hash_nodes);
printf("Unique terms         :%d\n", unique_terms);
printf("Term occurences      :%I64d\n", term_occurences);
printf("Mem used for docIDs  :%I64d\n", bytes_to_store_docids);
printf("Mem allocd for docIDs:%I64d\n", bytes_allocated_for_docids);
printf("DOCID mem utilisation:%2.2f%%\n", ((double)bytes_to_store_docids / (double)bytes_allocated_for_docids) * 100);
}