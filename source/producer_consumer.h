/*
	PRODUCER_CONSUMER.H
	-------------------
*/
#ifndef PRODUCER_CONSUMER_H_
#define PRODUCER_CONSUMER_H_

#include "semaphores.h"
#include "critical_section.h"

#include <string.h>

/*
	class ANT_PRODUCER_CONSUMER
	---------------------------
*/
template <class ANT_PRODUCER_CONSUMER_OBJECT>
class ANT_producer_consumer
{
protected:
	ANT_semaphores *empty_count, *fill_count;
	ANT_critical_section *mutex;

	ANT_PRODUCER_CONSUMER_OBJECT *queue;
	long insertion_point, removal_point, queue_length;

public:
	ANT_producer_consumer(long queue_length);
	virtual ~ANT_producer_consumer();

	ANT_PRODUCER_CONSUMER_OBJECT *add(ANT_PRODUCER_CONSUMER_OBJECT *object);
	ANT_PRODUCER_CONSUMER_OBJECT *remove(ANT_PRODUCER_CONSUMER_OBJECT *destination);
} ;

/*
	ANT_PRODUCER_CONSUMER::ANT_PRODUCER_CONSUMER()
	----------------------------------------------
*/
template <class ANT_PRODUCER_CONSUMER_OBJECT>
ANT_producer_consumer <ANT_PRODUCER_CONSUMER_OBJECT>::ANT_producer_consumer(long queue_length)
{
insertion_point = removal_point = 0;
this->queue_length = queue_length;
queue = new ANT_PRODUCER_CONSUMER_OBJECT[queue_length];
mutex = new ANT_critical_section;
empty_count = new ANT_semaphores(queue_length, queue_length);
fill_count = new ANT_semaphores(0, queue_length);
}

/*
	ANT_PRODUCER_CONSUMER::~ANT_PRODUCER_CONSUMER()
	-----------------------------------------------
*/
template <class ANT_PRODUCER_CONSUMER_OBJECT>
ANT_producer_consumer <ANT_PRODUCER_CONSUMER_OBJECT>::~ANT_producer_consumer()
{
delete [] queue;
delete mutex;
delete empty_count;
delete fill_count;
}

/*
	ANT_PRODUCER_CONSUMER::ADD()
	----------------------------
*/
template <class ANT_PRODUCER_CONSUMER_OBJECT>
ANT_PRODUCER_CONSUMER_OBJECT *ANT_producer_consumer <ANT_PRODUCER_CONSUMER_OBJECT>::add(ANT_PRODUCER_CONSUMER_OBJECT *object)
{
empty_count->enter();
mutex->enter();
memcpy(queue + insertion_point, object, sizeof(*object));
insertion_point = (insertion_point + 1) % queue_length;
mutex->leave();
fill_count->leave();

return object;
}

/*
	ANT_PRODUCER_CONSUMER::REMOVE()
	-------------------------------
*/
template <class ANT_PRODUCER_CONSUMER_OBJECT>
ANT_PRODUCER_CONSUMER_OBJECT *ANT_producer_consumer <ANT_PRODUCER_CONSUMER_OBJECT>::remove(ANT_PRODUCER_CONSUMER_OBJECT *destination)
{
fill_count->enter();
mutex->enter();
memcpy(destination, queue + removal_point, sizeof(*destination));
removal_point = (removal_point + 1) % queue_length;
mutex->leave();
empty_count->leave();

return destination;
}

#endif /* PRODUCER_CONSUMER_H_ */
