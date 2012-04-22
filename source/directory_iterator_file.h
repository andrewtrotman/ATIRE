/*
	DIRECTORY_ITERATOR_FILE.H
	-------------------------
*/
#ifndef DIRECTORY_ITERATOR_FILE_H_
#define DIRECTORY_ITERATOR_FILE_H_

#ifdef _MSC_VER
	#include <windows.h>
	#define PATH_MAX MAX_PATH
#else
	#include <limits.h>
#endif

#include "directory_iterator.h"

/*
	class ANT_DIRECTORY_ITERATOR_FILE
	---------------------------------
*/
class ANT_directory_iterator_file : public ANT_directory_iterator
{
protected:
	char *file, *document_start, *document_end;
	char filename[PATH_MAX];
	ANT_directory_iterator *source;
	int scrubbing;

protected:
	ANT_directory_iterator_object *read_entire_file(ANT_directory_iterator_object *object);
	char *get_next_file(void);
	void initialise(void);

public:
	ANT_directory_iterator_file(char *file, long get_file = 0);
	ANT_directory_iterator_file(ANT_directory_iterator *source, long get_file = 0);
	virtual ~ANT_directory_iterator_file() { delete [] file; delete source; }

	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object);
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);
} ;



#endif /* DIRECTORY_ITERATOR_FILE_H_ */
