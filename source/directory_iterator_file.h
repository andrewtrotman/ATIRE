/*
	DIRECTORY_ITERATOR_FILE.H
	-------------------------
*/
#ifndef DIRECTORY_ITERATOR_FILE_H_
#define DIRECTORY_ITERATOR_FILE_H_

#ifdef _MSC_VER
	#include <windows.h>
#else
	#include <limits.h>
	#define MAX_PATH PATH_MAX
#endif

#include "directory_iterator.h"

/*
	class ANT_DIRECTORY_ITERATOR_FILE
	---------------------------------
*/
class ANT_directory_iterator_file : public ANT_directory_iterator
{
private:
	char *file, *document_start, *document_end;
	char filename[PATH_MAX];

public:
	ANT_directory_iterator_file(char *file) : ANT_directory_iterator() { this->file = file; }
	virtual ~ANT_directory_iterator_file() {}

	virtual char *first(char *wildcard);
	virtual char *next(void);
	virtual char *read_entire_file(long long *len = 0);
} ;

#endif /* DIRECTORY_ITERATOR_FILE_H_ */
