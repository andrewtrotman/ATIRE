/*
	SEARCH_ENGINE_RESULT_ITERATOR.H
	-------------------------------
*/
#ifndef SEARCH_ENGINE_RESULT_ITERATOR_H_
#define SEARCH_ENGINE_RESULT_ITERATOR_H_

class ANT_search_engine;
class ANT_search_engine_result;
/*
	class ANT_SEARCH_ENGINE_RESULT_ITERATOR
	---------------------------------------
*/
class ANT_search_engine_result_iterator
{
private:
	ANT_search_engine *search_engine;
	ANT_search_engine_result *result;
	long long results_list_length;
	long long current;
#ifdef FILENAME_INDEX
	char *filename_buffer;
#endif

public:
#ifdef FILENAME_INDEX
	ANT_search_engine_result_iterator() { filename_buffer = new char [1024 * 8];}
	virtual ~ANT_search_engine_result_iterator() { delete [] filename_buffer; }

	virtual char *first(ANT_search_engine *engine, long long start = 0);	// start is the position in the results list from which to start (counting from 0)
	virtual char *next(void);
#else
	ANT_search_engine_result_iterator() {}
	virtual ~ANT_search_engine_result_iterator() {}

	virtual long long first(ANT_search_engine *engine, long long start = 0);	// start is the position in the results list from which to start (counting from 0)
	virtual long long next(void);
#endif
} ;

#endif /* SEARCH_ENGINE_RESULT_ITERATOR_H_ */
