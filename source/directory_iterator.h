/*
	DIRECTORY_ITERATOR.H
	--------------------
*/
#ifndef DIRECTORY_ITERATOR_H_
#define DIRECTORY_ITERATOR_H_

#ifndef START
	#ifdef LOGGING
		#define START printf("%ss %lld\n", message, clock->start_timer())
		#define END printf("%se %lld\n", message, clock->start_timer())
		#define STARTV(m) printf("%s %lu s %lld\n", m, id, clock->start_timer())
		#define ENDV(m) printf("%s %lu e %lld\n", m, id, clock->start_timer())
	#else
		#define START //
		#define END //
		#define STARTV(m) // m
		#define ENDV(m) // m
	#endif
#endif

#include "directory_iterator_object.h"

class ANT_directory_iterator_internals;
class ANT_stats;

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
	long get_file;

private:
	char *construct_full_path(char *filename);

public:
	ANT_directory_iterator(char *wildcard = "", long get_file = 0);
	virtual ~ANT_directory_iterator();

	/*
		These routines return the state of the next file in the
		ANT_directory_iterator_object and thus the only state the ANT_directory_iterator
		object holds is the current location in the directory.  It is pointless moving
		that state into the ANT_directory_iterator_object because if you want two differnt
		states in the current directory you can just create a second iterator.
	*/
	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object);
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);

	/*
		blah de blah
	*/
	char *message;
	unsigned long long wait_input_time;
	unsigned long long wait_output_time;
	unsigned long long process_time;
	ANT_stats *clock;
	bool printed;
} ;

#endif  /* DIRECTORY_ITERATOR_H_ */

