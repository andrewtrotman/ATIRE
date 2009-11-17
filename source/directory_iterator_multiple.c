/*
	DIRECTORY_ITERATOR_MULTIPLE.C
	-----------------------------
*/
#include <stdio.h>
#include <string.h>
#include "str.h"
#include "semaphores.h"
#include "critical_section.h"
#include "directory_iterator_multiple.h"
#include "directory_iterator_multiple_internals.h"

/*
	ANT_DIRECTORY_ITERATOR_MULTIPLE::ANT_DIRECTORY_ITERATOR_MULTIPLE()
	------------------------------------------------------------------
*/
ANT_directory_iterator_multiple::ANT_directory_iterator_multiple()
{
sources_length = sources_used = 0;
sources = NULL;
queue = NULL;
thread_details = NULL;

mutex = NULL;
empty_count = NULL;
fill_count = NULL;
}

/*
	ANT_DIRECTORY_ITERATOR_MULTIPLE::~ANT_DIRECTORY_ITERATOR_MULTIPLE()
	-------------------------------------------------------------------
*/
ANT_directory_iterator_multiple::~ANT_directory_iterator_multiple()
{
//ANT_directory_iterator_multiple_internals *current;

delete [] sources;

/*
for (current = thread_details; current < thread_details + sources_used; current++)
	delete [] current->wildcard;
*/

delete [] thread_details;
delete [] queue;

delete mutex;
delete empty_count;
delete fill_count;
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
	sources_length += 8;
	memory = new ANT_directory_iterator *[sources_length];
	memcpy(memory, sources, sources_used * sizeof(ANT_directory_iterator *));
	delete [] sources;
	sources = memory;

	delete [] thread_details;
	thread_details = new ANT_directory_iterator_multiple_internals[sources_length];

	queue_length = sources_length * 2;
	delete [] queue;
	queue = new ANT_directory_iterator_object [queue_length];
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

for (got = my->iterator->first(&my->file_object, my->get_file); got != NULL; got = my->iterator->next(&my->file_object, my->get_file))
	{
	empty_count->enter();
	mutex->enter();
	memcpy(queue + insertion_point, &my->file_object, sizeof(*queue));
	insertion_point = (insertion_point + 1) % queue_length;
	mutex->leave();
	fill_count->leave();
	}

/*
	At this point the thread is finished, but we have the remaining problem of
	signaling back to the consumer that all the producers are done.

	The obvious way to do this is to shove an empty element into the list
	and to count them at the consumer's end
*/
empty_count->enter();
mutex->enter();
queue[insertion_point].filename[0] = '\0';
insertion_point = (insertion_point + 1) % queue_length;
mutex->leave();
fill_count->leave();

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
insertion_point = removal_point = 0;
active_threads = sources_used;

/*
	Set up the semaphores
*/
mutex = new ANT_critical_section;
empty_count = new ANT_semaphores(queue_length, queue_length);
fill_count = new ANT_semaphores(0, queue_length);

/*
	Start each thread
*/
instance = 0;
for (current = thread_details; current < thread_details + sources_used; current++)
	{
	current->iterator = sources[instance];
	current->parent = this;
//	current->wildcard = strnew(wildcard);
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
fill_count->enter();
mutex->enter();
memcpy(object, queue + removal_point, sizeof(*object));
removal_point = (removal_point + 1) % queue_length;
mutex->leave();
empty_count->leave();

if (*object->filename == '\0')
	{
	/*
		One of the sources has dried-up so we reduce the number active and then get the next
	*/
	active_threads--;
	if (active_threads > 0)
		return next(object, get_file);
	}

return active_threads != 0 ? object : NULL;
}

