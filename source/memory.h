/*
	MEMORY.H
	--------
*/
#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdio.h>
#include <stdlib.h>
#include <new>

#include "critical_section.h"

#if defined(_WIN64) || (__SIZEOF_POINTER__ == 8) || (defined(__APPLE__) && (_LP64 == 1))
	const long long ANT_memory_block_size_for_allocation = 1024 * 1024 * 1024;
#elif defined(_WIN32) || (__SIZEOF_POINTER__ == 4) || defined(__APPLE__) || (__WORDSIZE == 32)
	const long long ANT_memory_block_size_for_allocation = 64 * 1024 * 1024;
#else
	const long long ANT_memory_block_size_for_allocation = 1024 * 1024 * 1024;
#endif

/*
	class ANT_MEMORY
	----------------
*/
class ANT_memory
{
private:
	ANT_critical_section critter;
	char *chunk, *at, *chunk_end;
	long long used;
	long long allocated;
	long long block_size;
	long has_large_pages;
	size_t short_page_size, large_page_size;

protected:
	void *alloc(long long *size);
	void dealloc(char *buffer);
	void *get_chained_block(long long bytes);
#ifdef _MSC_VER
	long set_privilege(char *priv_name, long enable);
#endif

public:
	ANT_memory(long long block_size_for_allocation = ANT_memory_block_size_for_allocation);
	~ANT_memory();

	void *malloc(long long bytes);
	void *synchronised_malloc(long long bytes);
	long long bytes_allocated(void) { return allocated; }
	long long bytes_used(void) { return used; }
	void realign(void);
	void rewind(void);
} ;

/*
	ANT_MEMORY::MALLOC()
	--------------------
*/
inline void *ANT_memory::malloc(long long bytes)
{
void *ans;

if (chunk == NULL || at + bytes > chunk_end)
	if (get_chained_block(bytes) == NULL)
		exit(printf("ANT:Out of memory:%lld bytes requested %lld bytes used %lld bytes allocated\n", (long long)bytes, (long long)used, (long long)allocated));

ans = at;
at += bytes;
used += bytes;

return ans;
}

/*
	ANT_MEMORY::SYNCHRONISED_MALLOC()
	---------------------------------
*/
inline void *ANT_memory::synchronised_malloc(long long bytes)
{
void *ans;

critter.enter();
ans = malloc(bytes);
critter.leave();

return ans;
}

#endif  /* MEMORY_H_ */
