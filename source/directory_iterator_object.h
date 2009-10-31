/*
	DIRECTORY_ITERATOR_OBJECT.H
	---------------------------
	This is the result of a call to first() and next() in one of the ANT_directory_iterator classes.
	Although the state could be held internally, that is a problem when it comes to prefetching in 
	one thread and processing in another
*/
#ifndef DIRECTORY_ITERATOR_OBJECT_H_
#define DIRECTORY_ITERATOR_OBJECT_H_

#ifdef _MSC_VER
	#include <windows.h>
	#define PATH_MAX MAX_PATH
#else
	#include <limits.h>
#endif

/*
	class ANT_DIRECTORY_ITERATOR_OBJECT
	-----------------------------------
*/
class ANT_directory_iterator_object
{
public:
	char filename[MAX_PATH];		// name of the file
	char *file;						// contents of the file
	long long length;				// length of the file
} ;

#endif /* DIRECTORY_ITERATOR_OBJECT_H_ */
