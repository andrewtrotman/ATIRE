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
	static long tid;

protected:
	char *document_start, *document_end;

	char *primary_buffer, *secondary_buffer;
	char **buffer_to_read_into, **buffer_to_read_from;
	long long *end_of_buffer, *end_of_second_buffer;
	long long position_of_end_of_buffer, position_of_end_of_second_buffer;

	long long primary_buffer_used, secondary_buffer_used;

	ANT_instream *source;
	long auto_file_id;
	char **doc_tag;
	char **docno_tag;

private:
	void free_tag();

protected:
	ANT_directory_iterator_object *read_entire_file(ANT_directory_iterator_object *object);
	long long read(char *destination, long long length);


public:
	ANT_directory_iterator_file_buffered(ANT_instream *instream, long get_file = 0);
	virtual ~ANT_directory_iterator_file_buffered();

	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object) { return next(object); }
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);

	void set_tags(char *doc_name, char *docno_name);
} ;



#endif /* DIRECTORY_ITERATOR_FILE_BUFFERED_H_ */
