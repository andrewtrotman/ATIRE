/*
	DIRECTORY_ITERATOR_PKZIP.H
	--------------------------
*/
#ifndef DIRECTORY_ITERATOR_PKZIP_H_
#define DIRECTORY_ITERATOR_PKZIP_H_

#include "directory_iterator.h"

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

private:
	void read_and_forget(unsigned long long bytes);
	unsigned long long read_central_directory_header(void);

public:
	ANT_directory_iterator_pkzip(char *filename);
	~ANT_directory_iterator_pkzip();
	
	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object, char *wildcard, long get_file = 0);
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object, long get_file = 0);
} ;


#endif /* DIRECTORY_ITERATOR_PKZIP_H_ */
