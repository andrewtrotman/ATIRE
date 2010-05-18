/*
	DIRECTORY_ITERATOR_MULTIPLE.H
	-----------------------------
*/
#ifndef DIRECTORY_ITERATOR_MULTIPLE_H_
#define DIRECTORY_ITERATOR_MULTIPLE_H_

#include "directory_iterator.h"
#include "producer_consumer.h"

class ANT_directory_iterator_multiple_internals;

/*
	class ANT_DIRECTORY_ITERATOR_MULTIPLE
	-------------------------------------
*/
class ANT_directory_iterator_multiple : public ANT_directory_iterator
{
private:
	static const long sources_growth_factor = 8;

protected:
	ANT_directory_iterator **sources;
	ANT_directory_iterator_multiple_internals *thread_details;
	long sources_used, sources_length, current_source;
	long active_threads;
	ANT_producer_consumer <ANT_directory_iterator_object> *producer;
	ANT_critical_section mutex;

protected:
	static void *bootstrap(void *param);
	void produce(ANT_directory_iterator_multiple_internals *internals);

public:
	ANT_directory_iterator_multiple(long get_file = 0);
	virtual ~ANT_directory_iterator_multiple();

	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object);
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);

	void add_iterator(ANT_directory_iterator *iterator);
} ;


#endif /* DIRECTORY_ITERATOR_MULTIPLE_H_ */
