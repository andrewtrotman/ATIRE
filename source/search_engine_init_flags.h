/*
	SEARCH_ENGINE_INIT_FLAGS.H
	--------------------------
*/
#ifndef SEARCH_ENGINE_INIT_FLAGS_H_
#define SEARCH_ENGINE_INIT_FLAGS_H_

#include <string.h>

/*
	class ANT_SEARCH_ENGINE_INIT_FLAGS
	----------------------------------
*/
class ANT_search_engine_init_flags
{
private:
	unsigned char *array;
	long long length;

public:
	ANT_search_engine_init_flags() { array = NULL; }
	~ANT_search_engine_init_flags() { delete [] array; }

	void init(long long documents)
		{
		delete [] array;
		array = new unsigned char [(size_t)(length = documents)];
		rewind();
		}

	void set_length(long long new_length) { length = new_length; }
	void resize(long long new_length) { length = new_length; }
	void rewind(void) { memset(array, 0, (size_t)(sizeof(*array) * length)); }

	inline unsigned char get(long long index) { return array[index]; }
	inline void set(long long index) { array[index] = 1; }
	inline void unset(long long index) { array[index] = 0; }
} ;

#endif /* SEARCH_ENGINE_INIT_FLAGS_H_ */
