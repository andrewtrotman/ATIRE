/*
	MEMORY.H
	--------
*/

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdlib.h>
#include <new.h>

class ANT_memory
{
friend int main(int argc, char *argv[]);			// delete this line
private:
	char *chunk, *at, *chunk_end;
	long long used;
	long long allocated;

public:
	ANT_memory();
	~ANT_memory();

	void *malloc(size_t bytes);
	long long bytes_allocated(void) { return allocated; }
	long long bytes_used(void) { return used; }
} ;

/*
	ANT_MEMORY::MALLOC()
	--------------------
*/
inline void *ANT_memory::malloc(size_t bytes)
{
void *ans;
#pragma omp critical
	{
	if (chunk == NULL)
		{
		allocated = 1024 * 1024 * 1024;
		at = chunk = new (std::nothrow) char [(size_t)allocated];
		if (chunk == NULL)
			exit(printf("Out of memory:%I64d bytes requested\n", allocated));
		chunk_end = chunk + allocated;
		}
	ans = at;
	at += bytes;
	if (at > chunk_end)
		exit(printf("Out of memory:%I64d bytes requested %I64d bytes used\n", (long long)bytes, used));
	used += bytes;
	}
return ans;
}

#endif __MEMORY_H__

