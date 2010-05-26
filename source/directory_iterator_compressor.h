/*
	DIRECTORY_ITERATOR_COMPRESSOR.H
	-------------------------------
*/
#ifndef DIRECTORY_ITERATOR_COMPRESSOR_H_
#define DIRECTORY_ITERATOR_COMPRESSOR_H_

#include "directory_iterator.h"
#include "producer_consumer.h"

class ANT_compression_text_factory;

/*
	class ANT_DIRECTORY_ITERATOR_COMPRESSOR
	---------------------------------------
*/
class ANT_directory_iterator_compressor : public ANT_directory_iterator
{
private:
	ANT_producer_consumer <ANT_directory_iterator_object> *store;
	ANT_directory_iterator *source;
//	long get_file;		// also in the parent object so removed from this object
	long threads;
	ANT_critical_section mutex;
	ANT_compression_text_factory *compressor;

private:
	void work_one(ANT_compression_text_factory *compressor, ANT_directory_iterator_object *object);
	void work(void);
	static void *bootstrap(void *param);

public:
	ANT_directory_iterator_compressor(ANT_directory_iterator *source, long threads, ANT_compression_text_factory *compressor, long get_file = 0);
	~ANT_directory_iterator_compressor();

	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object);
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);
} ;


#endif /* DIRECTORY_ITERATOR_COMPRESSOR_H_ */
