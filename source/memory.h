/*
	MEMORY.H
	--------
*/
#ifndef MEMORY_H_
#define MEMORY_H_

#ifndef _MSC_VER
	#include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <new>

#if defined(__arm__)
	const long long ANT_memory_block_size_for_allocation = 32 * 1024 * 1024;
#elif defined(_WIN64) || (__SIZEOF_POINTER__ == 8) || (defined(__APPLE__) && (_LP64 == 1))
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
	long long memory_ceiling;
	size_t short_page_size, large_page_size;

protected:
	void *alloc(long long *size);
	void dealloc(char *buffer);
	void *get_chained_block(long long bytes);
#ifdef _MSC_VER
	long set_privilege(char *priv_name, long enable);
#endif

public:
	ANT_memory(long long block_size_for_allocation = ANT_memory_block_size_for_allocation, long long memory_ceiling = 0);
	~ANT_memory();

	void *malloc(long long bytes);
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

#ifdef __arm__
	realign();
#endif

if (chunk == NULL || at + bytes > chunk_end)
	if (get_chained_block(bytes) == NULL)
		{
#ifdef NEVER
		exit(printf("ANT:Out of memory:%lld bytes requested %lld bytes used %lld bytes allocated\n", (long long)bytes, (long long)used, (long long)allocated));
#endif
		return NULL;		// out of memory (of the soft limit on this object has been exceeded)
		}

ans = at;
at += bytes;
used += bytes;

return ans;
}

#endif  /* MEMORY_H_ */
