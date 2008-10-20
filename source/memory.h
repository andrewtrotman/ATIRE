/*
	MEMORY.H
	--------
*/

#ifndef __MEMORY_H__
#define __MEMORY_H__

class ANT_memory
{
private:
	char *chunk, *at, *chunk_end;
	long long used;
	long long allocated;

public:
	ANT_memory();
	~ANT_memory();

	void *malloc(size_t bytes);
} ;

#endif __MEMORY_H__

