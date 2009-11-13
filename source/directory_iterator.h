/*
	DIRECTORY_ITERATOR.H
	--------------------
*/
#ifndef DIRECTORY_ITERATOR_H_
#define DIRECTORY_ITERATOR_H_

#include "directory_iterator_object.h"

class ANT_directory_iterator_internals;

/*
	class ANT_DIRECTORY_ITERATOR
	----------------------------
*/
class ANT_directory_iterator
{
public:
	enum { FILENAME_ONLY = 0, READ_FILE = 1 };

protected:
	ANT_directory_iterator_internals *internals;
	char wildcard[PATH_MAX];

private:
	char *construct_full_path(char *destination, char *filename);

public:
	ANT_directory_iterator(char *wildcard = "");
	virtual ~ANT_directory_iterator();

	/*
		These routines return the state of the next file in the
		ANT_directory_iterator_object and thus the only state the ANT_directory_iterator
		object holds is the current location in the directory.  It is pointless moving
		that state into the ANT_directory_iterator_object because if you want two differnt
		states in the current directory you can just create a second iterator.
	*/
	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object, char *the_wildcard, long get_file = 0);
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object, long get_file = 0);
} ;

#endif  /* DIRECTORY_ITERATOR_H_ */

