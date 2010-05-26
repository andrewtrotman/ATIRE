/*
	DIRECTORY_ITERATOR_PREINDEX.H
	-----------------------------
*/
#ifndef DIRECTORY_ITERATOR_PREINDEX_H_
#define DIRECTORY_ITERATOR_PREINDEX_H_

#include "directory_iterator.h"
#include "directory_iterator_object.h"
#include "producer_consumer.h"

/*
	class ANT_DIRECTORY_ITERATOR_PREINDEX
	-------------------------------------
*/
class ANT_directory_iterator_preindex : public ANT_directory_iterator
{
private:
	ANT_producer_consumer <ANT_directory_iterator_object> *store;
	ANT_directory_iterator *source;
	long threads;
	ANT_critical_section mutex;

private:
	void work_one(ANT_directory_iterator_object *object);
	void work(void);
	static void *bootstrap(void *param);

public:
	ANT_directory_iterator_preindex(ANT_directory_iterator *source, long threads, long get_file = 0);
	~ANT_directory_iterator_preindex();

	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object);
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);
} ;



#endif /* DIRECTORY_ITERATOR_PREINDEX_H_ */
