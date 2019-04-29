/*
	DIRECTORY_ITERATOR_TREC_RECURSIVE.H
	-----------------------------------
*/
#ifndef DIRECTORY_ITERATOR_TREC_RECURSIVE_H_
#define DIRECTORY_ITERATOR_TREC_RECURSIVE_H_

#include "directory_iterator.h"

class ANT_instream;
class ANT_memory;
class ANT_directory_iterator_file_buffered;

/*
	class ANT_DIRECTORY_ITERATOR_TREC_RECURSIVE
	-------------------------------------------
*/
class ANT_directory_iterator_trec_recursive : public ANT_directory_iterator
{
protected:
	char *source;
	ANT_instream *file_stream;
	ANT_instream *decompressor;
	ANT_instream *instream_buffer;
	ANT_instream *scrubber;
	ANT_directory_iterator *filename_provider;
	ANT_directory_iterator_file_buffered *detrecer;
	ANT_memory *memory;
	ANT_directory_iterator_object *more_files;
	long first_time;
	long long scrubbing_options;

protected:
	ANT_directory_iterator *new_provider(char *filename);

public:
	ANT_directory_iterator_trec_recursive(char *source, long get_file = 0, long long scrubbing_options = 0);
	virtual ~ANT_directory_iterator_trec_recursive();

	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object) { return next(object); }
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);

	void set_tags(char *doc_name, char *docno_name);
} ;

#endif /* DIRECTORY_ITERATOR_TREC_RECURSIVE_H_ */
