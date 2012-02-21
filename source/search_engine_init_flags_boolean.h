/*
	SEARCH_ENGINE_INIT_FLAGS_BOOLEAN.H
	----------------------------------
*/
#ifndef SEARCH_ENGINE_INIT_FLAGS_BOOLEAN_H_
#define SEARCH_ENGINE_INIT_FLAGS_BOOLEAN_H_

#include <stdio.h>			// for puts()
#include <string.h>
#include "bitstring.h"

/*
	class ANT_SEARCH_ENGINE_INIT_FLAGS_BOOLEAN
	------------------------------------------
*/
class ANT_search_engine_init_flags_boolean : private ANT_bitstring
{
private:
	long long length, initial_length;

public:
	ANT_search_engine_init_flags_boolean() : ANT_bitstring() { }
	~ANT_search_engine_init_flags_boolean() { }

	void init(long long documents)
		{
		ANT_bitstring::set_length(initial_length = length = documents);
		rewind();
		}

	void set_length(long long new_length)
		{
		if (new_length > initial_length)
			puts("bitstring initial length exceeded");

		length = new_length;
		ANT_bitstring::unsafe_set_length(new_length);
		}

	void resize(long long new_length)
		{
		if (new_length > initial_length)
			printf("bitstring initial length exceeded (%lld > %lld)", new_length, initial_length);

		length = new_length;
		ANT_bitstring::unsafe_set_length(new_length);
		}

	void rewind(void) { ANT_bitstring::zero(); }

	inline long get(long long index) { return ANT_bitstring::unsafe_getbit(index); }
	inline void set(long long index) { ANT_bitstring::unsafe_setbit(index); }
	inline void unset(long long index) { ANT_bitstring::unsafe_unsetbit(index); }
} ;

#endif /* SEARCH_ENGINE_INIT_FLAGS_BOOLEAN_H_ */
