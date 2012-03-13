/*
	DIRECTORY_ITERATOR_DEFLATE.H
	----------------------------
*/
#ifndef DIRECTORY_ITERATOR_DEFLATE_H_
#define DIRECTORY_ITERATOR_DEFLATE_H_

#include "directory_iterator.h"

class ANT_compress_text;

/*
	class ANT_DIRECTORY_ITERATOR_DEFLATE
	------------------------------------
	This class takes as input the output of another directory_iterator and decomresses it. The
	use case is when you have a directory full of .gz files and you want to recursively iterate
	over directory decompressing each file.
*/
class ANT_directory_iterator_deflate : public ANT_directory_iterator
{
public:
	enum {BINARY, TEXT};		// in TEXT mode we check there are no '\0' chars in the decompressed output

protected:
	static const long long INITIAL_LARGEST_DECOMPRESSED_FILE_SIZE = 1024 * 1024;		// initial assumption on file size

protected:
	ANT_directory_iterator *source;				// where the compressed files are coming from
	ANT_compress_text *decompressor;			// the decompressors that decompresses the files
	long long largest_decompressed_file;		// block size for allocation on decompression
	long mode;

protected:
	virtual ANT_directory_iterator_object *process(ANT_directory_iterator_object *object);

public:
	ANT_directory_iterator_deflate(ANT_directory_iterator *source, long mode = BINARY);
	virtual ~ANT_directory_iterator_deflate();

	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object) { return process(source->first(object)); }
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object) { return process(source->next(object)); }
} ;


#endif /* DIRECTORY_ITERATOR_DEFLATE_H_ */
