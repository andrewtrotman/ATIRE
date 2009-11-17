/*
	DIRECTORY_ITERATOR_MULTIPLE.H
	-----------------------------
*/
#ifndef DIRECTORY_ITERATOR_MULTIPLE_H_
#define DIRECTORY_ITERATOR_MULTIPLE_H_

#include "directory_iterator.h"

class ANT_directory_iterator_multiple_internals;
class ANT_semaphores;
class ANT_critical_section;

/*
	class ANT_DIRECTORY_ITERATOR_MULTIPLE
	-------------------------------------
*/
class ANT_directory_iterator_multiple : public ANT_directory_iterator
{
protected:
	ANT_directory_iterator **sources;
	ANT_directory_iterator_multiple_internals *thread_details;
	ANT_directory_iterator_object *queue;
	long insertion_point, removal_point, queue_length;
	long sources_used, sources_length, current_source;
	long active_threads;
	ANT_semaphores *empty_count, *fill_count;
	ANT_critical_section *mutex;

protected:
	static void *bootstrap(void *param);
	void produce(ANT_directory_iterator_multiple_internals *internals);

public:
	ANT_directory_iterator_multiple();
	virtual ~ANT_directory_iterator_multiple();

	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object, long get_file = 0);
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object, long get_file = 0);

	void add_iterator(ANT_directory_iterator *iterator);
} ;


#endif /* DIRECTORY_ITERATOR_MULTIPLE_H_ */
