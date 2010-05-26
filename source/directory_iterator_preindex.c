/*
	DIRECTORY_ITERATOR_PREINDEX.C
	-----------------------------
*/
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include "directory_iterator_preindex.h"
#include "threads.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_DIRECTORY_ITERATOR_PREINDEX::ANT_DIRECTORY_ITERATOR_PREINDEX()
	------------------------------------------------------------------
*/
ANT_directory_iterator_preindex::ANT_directory_iterator_preindex(ANT_directory_iterator *source, long threads, long get_file) : ANT_directory_iterator("", get_file)
{
this->threads = threads;
this->source = source;
store = new ANT_producer_consumer <ANT_directory_iterator_object> (threads);
}

/*
	ANT_DIRECTORY_ITERATOR_PREINDEX::~ANT_DIRECTORY_ITERATOR_PREINDEX()
	-------------------------------------------------------------------
*/
ANT_directory_iterator_preindex::~ANT_directory_iterator_preindex()
{
delete store;
}

/*
	ANT_DIRECTORY_ITERATOR_PREINDEX::WORK_ONE()
	-------------------------------------------
*/
void ANT_directory_iterator_preindex::work_one(ANT_directory_iterator_object *object)
{
store->add(object);
}

/*
	ANT_DIRECTORY_ITERATOR_PREINDEX::WORK()
	---------------------------------------
*/
void ANT_directory_iterator_preindex::work()
{
ANT_directory_iterator_object object;

while (source->next(&object) != NULL)
	work_one(&object);

object.filename[0] = '\0';
store->add(&object);
}

/*
	ANT_DIRECTORY_ITERATOR_PREINDEX::BOOTSTRAP()
	--------------------------------------------
*/
void *ANT_directory_iterator_preindex::bootstrap(void *param)
{
((ANT_directory_iterator_preindex *)param)->work();

return NULL;
}

/*
	ANT_DIRECTORY_ITERATOR_PREINDEX::FIRST()
	----------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_preindex::first(ANT_directory_iterator_object *object)
{
long instance;

/*
	The first object we get the slow way and then we start all the threads.  We do this
	to get around the problem of not knowing who should call first().
*/
if (source->first(object) != NULL)
	work_one(object);
else
	{
	/*
		We're at EOF on the first document
	*/
	object->filename[0] = '\0';
	store->add(object);
	}

/*
	Now we start each thread.
*/
for (instance = 0; instance < threads; instance++)
	ANT_thread(bootstrap, this);

return next(object);
}

/*
	ANT_DIRECTORY_ITERATOR_PREINDEX::NEXT()
	---------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_preindex::next(ANT_directory_iterator_object *object)
{
long finished = FALSE;

mutex.enter();
	if (threads <= 0)
		finished = TRUE;
	else
		do
			{
			store->remove(object);
			if (*object->filename != '\0')
				break;
			else
				if (--threads <= 0)
					finished = TRUE;				// all sources have dried up
			}
		while (!finished);
mutex.leave();

if (finished)
	return NULL;
else
	return object;
}
