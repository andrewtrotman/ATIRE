/*
	MEMORY.H
	--------
*/
#ifndef MEMORY_H_
#define MEMORY_H_

#if (defined(ANDROID) || defined(__ANDROID__))
	#include <stdlib.h>
	#include <unistd.h>
	#include <stdio.h>
#else
	#include <cstdio>
	#include <cstdlib>
	#include <new>
#endif

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

#ifdef __arm__
	realign();
#endif

if (chunk == NULL || at + bytes > chunk_end)
	if (get_chained_block(bytes) == NULL)
#if (defined(ANDROID) || defined(__ANDROID__))
		return 0;
		/*
		 	I guess the exit function will cause undefined behaviors when it is called in the JVM environment, so it was filtered in NDK
		 */
#else
		exit(printf("ANT:Out of memory:%lld bytes requested %lld bytes used %lld bytes allocated\n", (long long)bytes, (long long)used, (long long)allocated));
#endif

ans = at;
at += bytes;
used += bytes;

return ans;
}

#endif  /* MEMORY_H_ */
