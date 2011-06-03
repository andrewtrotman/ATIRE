/*
	DIRECTORY_ITERATOR_PKZIP.H
	--------------------------
*/
#ifndef DIRECTORY_ITERATOR_PKZIP_H_
#define DIRECTORY_ITERATOR_PKZIP_H_

#include "directory_iterator.h"
#include "directory_iterator_pkzip_internals.h"

class ANT_file;
class ANT_memory;

/*
	class ANT_DIRECTORY_ITERATOR_PKZIP
	----------------------------------
*/
class ANT_directory_iterator_pkzip : public ANT_directory_iterator
{
private:
	static const long PKZIP_BLOCK_SIZE = 1024;
	static const long PKZIP_METHOD_STORED = 0;
	static const long PKZIP_METHOD_DEFLATE = 8;
	static const long PKZIP_METHOD_BZ2 = 12;

private:
	ANT_file *file;
	ANT_memory *memory;
	unsigned char buffer[PKZIP_BLOCK_SIZE];
	unsigned long long directory_position;
	unsigned long long directory_length;
	unsigned long long directory_files;
	unsigned long long files_read;
	ANT_directory_iterator_pkzip_internals *internals;

	/*
		In the case of a zip stream we need to load the the offset and sizes of the files from the central directory
	*/
	long zip_stream;									// is the ZIP file a ZIP stream without sizes in the local_file_headers?
	ANT_directory_iterator_pkzip_internals::ANT_ZIP_file_stats *directory;

private:
	void read_and_forget(unsigned long long bytes);
	unsigned long long read_central_directory_header(void);
	void read_central_directory(void);
	ANT_directory_iterator_object *next_file(ANT_directory_iterator_object *object);

public:
	ANT_directory_iterator_pkzip(char *filename, long get_file = 0);
	~ANT_directory_iterator_pkzip();
	
	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object);
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);
} ;


#endif /* DIRECTORY_ITERATOR_PKZIP_H_ */
