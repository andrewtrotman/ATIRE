/*
	DIRECTORY_ITERATOR_TSV.H
	------------------------
	Specifically designed for the ClueWeb09 anchors, document name is the first
	then a tab then the url then the anchor texts
*/
#ifndef DIRECTORY_ITERATOR_TSV_H_
#define DIRECTORY_ITERATOR_TSV_H_

#include "directory_iterator_file.h"
#include "instream.h"

/*
	class ANT_DIRECTORY_ITERATOR_TSV
	--------------------------------
*/
class ANT_directory_iterator_tsv : public ANT_directory_iterator
{
private:
	static const long long buffer_size = 11 * 1024 * 1024;

	ANT_instream *source;
	unsigned char *buffer;
	long long position;
	long long end_of_buffer;

	long long read_line();

public:
	ANT_directory_iterator_tsv(ANT_instream *source, long get_file = 0) : ANT_directory_iterator("", get_file), source(source) {}
	virtual ~ANT_directory_iterator_tsv() {}

	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object);
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);
} ;

#endif /* DIRECTORY_ITERATOR_TSV_H_ */
