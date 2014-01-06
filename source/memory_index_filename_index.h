/*
	MEMORY_INDEX_FILENAME_INDEX.H
	-----------------------------
*/
#ifndef MEMORY_INDEX_FILENAME_INDEX_H_
#define MEMORY_INDEX_FILENAME_INDEX_H_

/*
	class ANT_MEMORY_INDEX_FILENAME_INDEX
	-------------------------------------
	This class implements a growable array that allocates members_per_chunk elements in each chunk and keeps
	an array of chunks.  It allocated each chunk one at a time, but grows the array of chunks at the rate of
	chunk_growth_rate per allocation.
*/
class ANT_memory_index_filename_index
{
protected:
	static const size_t members_per_chunk = 4 * 1024 * 1024;
	static const size_t chunk_growth_rate = 10;

protected:
	long chunks_allocated;
	long chunks_used;
	long members_used;
	long long **chunk;

public:
	ANT_memory_index_filename_index();
	~ANT_memory_index_filename_index();
	void add(long long value);
	long long *serialise(void);
	long long members(void) { return (long long)((chunks_used) * members_per_chunk + members_used); }

	void text_render(void);
} ;

#endif