/*
	DIRECTORY_ITERATOR_PREINDEX.C
	-----------------------------
*/
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include "directory_iterator_preindex.h"
#include "directory_iterator_preindex_internals.h"
#include "threads.h"
#include "memory.h"
#include "memory_index_one.h"
#include "index_document.h"
#include "stats.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

long ANT_directory_iterator_preindex::tid = 0;

/*
	ANT_DIRECTORY_ITERATOR_PREINDEX::ANT_DIRECTORY_ITERATOR_PREINDEX()
	------------------------------------------------------------------
*/
ANT_directory_iterator_preindex::ANT_directory_iterator_preindex(ANT_directory_iterator *source, long segmentation, unsigned long readability_measure, long stemmer, ANT_index_document *index_document_method, ANT_memory_index *final_index, long threads, long get_file) : ANT_directory_iterator("", get_file)
{
this->threads = threads;
this->source = source;
this->final_index = final_index;

this->segmentation = segmentation;
this->readability_measure = readability_measure;
this->stemmer = stemmer;

this->index_document = index_document_method;

store = new ANT_producer_consumer <ANT_directory_iterator_object> (threads);

clock = new ANT_stats(new ANT_memory);

//printf("%sstart_upstream %lld\n", message, clock->start_timer());
}

/*
	ANT_DIRECTORY_ITERATOR_PREINDEX::~ANT_DIRECTORY_ITERATOR_PREINDEX()
	-------------------------------------------------------------------
*/
ANT_directory_iterator_preindex::~ANT_directory_iterator_preindex()
{
//printf("%send_upstream %lld\n", message, clock->start_timer());
delete store;
delete source;
}

/*
	ANT_DIRECTORY_ITERATOR_PREINDEX::WORK_ONE()
	-------------------------------------------
*/
void ANT_directory_iterator_preindex::work_one(ANT_directory_iterator_object *object, ANT_directory_iterator_preindex_internals *internals, long id)
{
long terms;

STARTV("preindex");

object->index = new ANT_memory_index_one(new ANT_memory(1024 * 1024), final_index);
terms = index_document->index_document(object->index, internals->stemmer, internals->segmentation, internals->readability, 1, object->file);
object->terms = terms;

store->add(object);
ENDV("preindex");
}

/*
	ANT_DIRECTORY_ITERATOR_PREINDEX::WORK()
	---------------------------------------
*/
void ANT_directory_iterator_preindex::work(ANT_directory_iterator_preindex_internals *internals)
{
ANT_directory_iterator_object object;
	static long idd = 0;
long id = idd++;

while (source->next(&object) != NULL)
	work_one(&object, internals, id);

object.filename = NULL;
store->add(&object);
}

/*
	ANT_DIRECTORY_ITERATOR_PREINDEX::BOOTSTRAP()
	--------------------------------------------
*/
void *ANT_directory_iterator_preindex::bootstrap(void *param)
{
ANT_directory_iterator_preindex_internals *internals;

internals = (ANT_directory_iterator_preindex_internals *)param;
internals->parent->work(internals);

delete internals;
return NULL;
}

/*
	ANT_DIRECTORY_ITERATOR_PREINDEX::FIRST()
	----------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_preindex::first(ANT_directory_iterator_object *object)
{
long instance;
ANT_directory_iterator_preindex_internals *next_thread;

/*
	The first object we get the slow way and then we start all the threads.  We do this
	to get around the problem of not knowing who should call first().
*/
next_thread = new ANT_directory_iterator_preindex_internals(this);
if (source->first(object) != NULL)
	work_one(object, next_thread, 0);
else
	{
	/*
		We're at EOF on the first document
	*/
	object->filename = NULL;
	store->add(object);
	}

delete next_thread;

/*
	Now we start each thread.
*/
for (instance = 0; instance < threads; instance++)
	{
	next_thread = new ANT_directory_iterator_preindex_internals(this);
	ANT_thread(bootstrap, next_thread);
	}

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
			if (object->filename != NULL)
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
