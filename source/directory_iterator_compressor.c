/*
	DIRECTORY_ITERATOR_COMPRESSOR.C
	-------------------------------
*/
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include "directory_iterator_compressor.h"
#include "compression_text_factory.h"
#include "threads.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_DIRECTORY_ITERATOR_COMPRESSOR::ANT_DIRECTORY_ITERATOR_COMPRESSOR()
	----------------------------------------------------------------------
*/
ANT_directory_iterator_compressor::ANT_directory_iterator_compressor(ANT_directory_iterator *source, long threads, ANT_compression_text_factory *compressor, long get_file) : ANT_directory_iterator("", get_file)
{
this->compressor = compressor;
this->threads = threads;
this->source = source;
store = new ANT_producer_consumer <ANT_directory_iterator_object> (threads);
}

/*
	ANT_DIRECTORY_ITERATOR_COMPRESSOR::~ANT_DIRECTORY_ITERATOR_COMPRESSOR()
	-----------------------------------------------------------------------
*/
ANT_directory_iterator_compressor::~ANT_directory_iterator_compressor()
{
delete store;
delete source;
delete compressor;
}

/*
	ANT_DIRECTORY_ITERATOR_COMPRESSOR::WORK_ONE()
	---------------------------------------------
*/
void ANT_directory_iterator_compressor::work_one(ANT_compression_text_factory *compressor, ANT_directory_iterator_object *object)
{
unsigned long size;

/*
	The length of the object does not include the '\0' on the end, which we want to store
	so that we don't need to '\0' terminate later.  The compressor will also need to store
	one byte at the beginning saying which scheme it should use to decompress.  So, we need
	to add 2 to the length we allocate and 1 the the length we compress.
*/
object->compressed = new (std::nothrow) char [(size_t)(size = compressor->space_needed_to_compress((unsigned long)object->length + 1))];		// +1 to include the '\0'
if (compressor->compress(object->compressed, &size, object->file, (unsigned long)(object->length + 1)) == NULL)
	exit(printf("Cannot compress document (name:%s)\n", object->filename));
object->compressed_length = size;
store->add(object);
}

/*
	ANT_DIRECTORY_ITERATOR_COMPRESSOR::WORK()
	-----------------------------------------
*/
void ANT_directory_iterator_compressor::work()
{
ANT_compression_text_factory *my_compressor;
ANT_directory_iterator_object object;

my_compressor = compressor->replicate();

while (source->next(&object) != NULL)
	work_one(my_compressor, &object);

object.filename = NULL;
store->add(&object); 

delete my_compressor;
}

/*
	ANT_DIRECTORY_ITERATOR_COMPRESSOR::BOOTSTRAP()
	----------------------------------------------
*/
void *ANT_directory_iterator_compressor::bootstrap(void *param)
{
ANT_directory_iterator_compressor *current;

current = (ANT_directory_iterator_compressor *)param;
current->work();

return NULL;
}

/*
	ANT_DIRECTORY_ITERATOR_COMPRESSOR::FIRST()
	------------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_compressor::first(ANT_directory_iterator_object *object)
{
long instance;

/*
	The first object we get the slow way and then we start all the threads.  We do this
	to get around the problem of not knowing who should call first().
*/
if (source->first(object) != NULL)
	work_one(compressor, object);
else
	{
	/*
		We're at EOF on the first document
	*/
	object->filename = NULL;
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
	ANT_DIRECTORY_ITERATOR_COMPRESSOR::NEXT()
	-----------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_compressor::next(ANT_directory_iterator_object *object)
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
