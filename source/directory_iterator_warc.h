/*
	DIRECTORY_ITERATOR_WARC.H
	-------------------------
*/
#ifndef DIRECTORY_ITERATOR_WARC_H_
#define DIRECTORY_ITERATOR_WARC_H_

#include "instream.h"
#include "directory_iterator.h"

/*
	class ANT_DIRECTORY_ITERATOR_WARC
	---------------------------------
*/
class ANT_directory_iterator_warc : public ANT_directory_iterator
{
private:
	static const long WARC_BLOCK_SIZE = 1024;

private:
	ANT_instream *source;
	unsigned char buffer[WARC_BLOCK_SIZE];

private:
	unsigned char *find_string(char *string, long string_length);
	void read_entire_file(ANT_directory_iterator_object *object);

public:
	ANT_directory_iterator_warc(ANT_instream *source, long get_file = 0) : ANT_directory_iterator("", get_file) { this->source = source; }
	virtual ~ANT_directory_iterator_warc() {}

	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object) { return next(object); }
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);
} ;

#endif /* DIRECTORY_ITERATOR_WARC_H_ */
