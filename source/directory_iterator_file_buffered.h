/*
	DIRECTORY_ITERATOR_FILE_BUFFERED.H
	----------------------------------
*/
#ifndef DIRECTORY_ITERATOR_FILE_BUFFERED_H_
#define DIRECTORY_ITERATOR_FILE_BUFFERED_H_



#ifdef _MSC_VER
	#include <windows.h>
	#define PATH_MAX MAX_PATH
#else
	#include <limits.h>
#endif

#include "directory_iterator.h"
class ANT_instream;

/*
	class ANT_DIRECTORY_ITERATOR_FILE_BUFFERED
	------------------------------------------
*/
class ANT_directory_iterator_file_buffered : public ANT_directory_iterator
{
private:
	static const size_t buffer_size = (16 * 1024 * 1024);

protected:
	char *document_start, *document_end;
	char *read_buffer;
	long long read_buffer_used;
	long asciiafy;
	ANT_instream *source;
	long auto_file_id;

protected:
	ANT_directory_iterator_object *read_entire_file(ANT_directory_iterator_object *object);
	long long read(char *destination, long long length);


public:
	ANT_directory_iterator_file_buffered(ANT_instream *instream, long get_file = 0);
	virtual ~ANT_directory_iterator_file_buffered();

	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object) { return next(object); }
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);
} ;



#endif /* DIRECTORY_ITERATOR_FILE_BUFFERED_H_ */
