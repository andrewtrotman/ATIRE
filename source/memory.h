/*
	MEMORY.H
	--------
*/

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdio.h>
#include <stdlib.h>
#include <new>

class ANT_memory
{
friend int main(int argc, char *argv[]);			// delete this line
private:
	char *chunk, *at, *chunk_end;
	long long used;
	long long allocated;
	long long block_size;

public:
	ANT_memory(long long block_size_for_allocation = 1024 * 1024 * 1024);
	~ANT_memory();

	void *malloc(size_t bytes);
	long long bytes_allocated(void) { return allocated; }
	long long bytes_used(void) { return used; }
	void realign(void) { allocated += (allocated % sizeof(long long) == 0) ? 0 : sizeof(long long) - allocated % sizeof(long long); }		//
} ;

/*
	ANT_MEMORY::MALLOC()
	--------------------
*/
inline void *ANT_memory::malloc(size_t bytes)
{
void *ans;

if (chunk == NULL)
	{
	allocated = block_size;
	at = chunk = new (std::nothrow) char [(size_t)allocated];
	if (chunk == NULL)
		exit(printf("Out of memory:%lld bytes requested\n", allocated));
	chunk_end = chunk + allocated;
	}
ans = at;
at += bytes;
if (at > chunk_end)
	exit(printf("Out of memory:%lld bytes requested %lld bytes used\n", (long long)bytes, used));
used += bytes;

return ans;
}

#endif __MEMORY_H__

