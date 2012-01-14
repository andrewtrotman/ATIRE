/*
	SEARCH_ENGINE_INIT_FLAGS_BOOLEAN.H
	----------------------------------
*/
#ifndef SEARCH_ENGINE_INIT_FLAGS_BOOLEAN_H_
#define SEARCH_ENGINE_INIT_FLAGS_BOOLEAN_H_

#include <string.h>
#include "bitstring.h"

/*
	class ANT_SEARCH_ENGINE_INIT_FLAGS_BOOLEAN
	------------------------------------------
*/
class ANT_search_engine_init_flags_boolean
{
private:
	ANT_bitstring array;
	long long length;

public:
	ANT_search_engine_init_flags_boolean() { }
	~ANT_search_engine_init_flags_boolean() { }

	void init(long long documents)
		{
		array.set_length(length = documents);
		rewind();
		}

	void set_length(long long new_length) { length = new_length; array.unsafe_set_length(new_length); }
	void rewind(void) { array.zero(); }

	inline long get(long long index) { return array.unsafe_getbit(index); }
	inline void set(long long index) { array.unsafe_setbit(index); }
	inline void unset(long long index) { array.unsafe_unsetbit(index); }
} ;

#endif /* SEARCH_ENGINE_INIT_FLAGS_BOOLEAN_H_ */
