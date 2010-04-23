/*
	MEMORY.H
	--------
*/
#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdio.h>
#include <stdlib.h>
#include <new>

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
	char *chunk, *at, *chunk_end;
	long long used;
	long long allocated;
	long long block_size;
	long has_large_pages;
	size_t short_page_size, large_page_size;

protected:
	void *alloc(long long *size);
	void *get_chained_block(long long bytes);
#ifdef _MSC_VER
	long set_privilege(char *priv_name, long enable);
#endif


public:
	ANT_memory(long long block_size_for_allocation = ANT_memory_block_size_for_allocation);
	~ANT_memory();

	void *malloc(long long bytes);
	long long bytes_allocated(void) { return allocated; }
	long long bytes_used(void) { return used; }
	/*
		realign() does two things.  First, it aligns the next block of memory on the correct boundary for the largest
		type we know about (a 64-bit long long) to avoid memory miss-alignment overheads.  Second, and as a consequence,
		it cache-line aligns the next memory allocation (Intel uses a 64-byte cache line) thus reducing the number of
		cache misses if we process the memory sequentially.
	*/
	void realign(void) { allocated += (allocated % sizeof(long long) == 0) ? 0 : sizeof(long long) - allocated % sizeof(long long); }
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
		exit(printf("ANT:Out of memory:%lld bytes requested %lld bytes used %lld bytes allocated\n", (long long)bytes, used, allocated));

ans = at;
at += bytes;
used += bytes;

return ans;
}

#endif  /* MEMORY_H_ */

