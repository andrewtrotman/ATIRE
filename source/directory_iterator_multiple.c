/*
	DIRECTORY_ITERATOR_MULTIPLE.C
	-----------------------------
*/
#include <stdio.h>
#include <string.h>
#include "str.h"
#include "producer_consumer.h"
#include "directory_iterator_multiple.h"
#include "directory_iterator_multiple_internals.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_DIRECTORY_ITERATOR_MULTIPLE::ANT_DIRECTORY_ITERATOR_MULTIPLE()
	------------------------------------------------------------------
*/
ANT_directory_iterator_multiple::ANT_directory_iterator_multiple()
{
sources_length = sources_used = 0;
sources = NULL;
thread_details = NULL;
producer = NULL;
}

/*
	ANT_DIRECTORY_ITERATOR_MULTIPLE::~ANT_DIRECTORY_ITERATOR_MULTIPLE()
	-------------------------------------------------------------------
*/
ANT_directory_iterator_multiple::~ANT_directory_iterator_multiple()
{
delete [] sources;
delete [] thread_details;
delete producer;
}

/*
	ANT_DIRECTORY_ITERATOR_MULTIPLE::ADD_ITERATOR()
	-----------------------------------------------
*/
void ANT_directory_iterator_multiple::add_iterator(ANT_directory_iterator *iterator)
{
ANT_directory_iterator **memory;

if (sources_used >= sources_length)
	{
	sources_length += sources_growth_factor;
	memory = new ANT_directory_iterator *[sources_length];
	memcpy(memory, sources, sources_used * sizeof(ANT_directory_iterator *));
	delete [] sources;
	sources = memory;

	delete [] thread_details;
	thread_details = new ANT_directory_iterator_multiple_internals[sources_length];
	}
sources[sources_used] = iterator;
sources_used++;
}

/*
	ANT_DIRECTORY_ITERATOR_MULTIPLE::PRODUCE()
	------------------------------------------
	This is the producer in the classic producer / consumer model
*/
void ANT_directory_iterator_multiple::produce(ANT_directory_iterator_multiple_internals *my)
{
ANT_directory_iterator_object *got;

/*
	Exhaust the supply
*/
for (got = my->iterator->first(&my->file_object, my->get_file); got != NULL; got = my->iterator->next(&my->file_object, my->get_file))
	producer->add(&my->file_object);

/*
	At this point the thread is finished, but we have the remaining problem of
	signaling back to the consumer that all the producers are done.

	The obvious way to do this is to shove an empty element into the list
	and to count them at the consumer's end
*/
my->file_object.filename[0] = '\0';
producer->add(&my->file_object);

/*
	Now the thread is done it'll die its natural death
*/
}

/*
	ANT_DIRECTORY_ITERATOR_MULTIPLE::BOOTSTRAP()
	--------------------------------------------
*/
void *ANT_directory_iterator_multiple::bootstrap(void *param)
{
ANT_directory_iterator_multiple_internals *current;

current = (ANT_directory_iterator_multiple_internals *)param;
current->thread_id = ANT_thread_id();

current->parent->produce(current);

return NULL;
}

/*
	ANT_DIRECTORY_ITERATOR_MULTIPLE::FIRST()
	----------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_multiple::first(ANT_directory_iterator_object *object, long get_file)
{
long instance;
ANT_directory_iterator_multiple_internals *current;

/*
	Set up this object
*/
producer = new ANT_producer_consumer<ANT_directory_iterator_object> (sources_used);
active_threads = sources_used;

/*
	Start each thread
*/
instance = 0;
for (current = thread_details; current < thread_details + sources_used; current++)
	{
	current->iterator = sources[instance];
	current->parent = this;
	current->get_file = get_file;
	ANT_thread(bootstrap, current);
	current->instance = instance++;
	current->files_read = 0;
	}

/*
	And then consume
*/
return next(object, get_file);
}

/*
	ANT_DIRECTORY_ITERATOR_MULTIPLE::NEXT()
	---------------------------------------
	This is the consumer in the classic producer / consumer model
*/
ANT_directory_iterator_object *ANT_directory_iterator_multiple::next(ANT_directory_iterator_object *object, long get_file)
{
long finished = FALSE;

mutex.enter();
	if (active_threads <= 0)
		finished = TRUE;
	else
		do
			{
			producer->remove(object);
			if (*object->filename != '\0')
				break;
			else
				if (--active_threads <= 0)
					finished = TRUE;				// all sources have dried up
			}
		while (!finished);
mutex.leave();

if (finished)
	return NULL;
else
	return object;
}

