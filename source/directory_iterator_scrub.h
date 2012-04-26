/*
	DIRECTORY_ITERATOR_SCRUB.H
	--------------------------
*/
#ifndef DIRECTORY_ITERATOR_SCRUB_H_
#define DIRECTORY_ITERATOR_SCRUB_H_

#include "directory_iterator.h"

/*
	class ANT_DIRECTORY_ITERATOR_SCRUB
	----------------------------------
*/
class ANT_directory_iterator_scrub : public ANT_directory_iterator
{
friend class ANT_instream_scrub;
friend class ANT_directory_iterator_deflate;

public:
	enum {
		NONE       = 0,
		NUL        = 1,
		NON_ASCII  = 2,
		UTF8       = 4
	};

private:
	ANT_directory_iterator *source;
	long long scrubbing;
	
	static void scrub(unsigned char *data, long long size, long long scrubbing);

public:
	ANT_directory_iterator_scrub(ANT_directory_iterator *source, long long scrubbing, long get_file = 0);
	~ANT_directory_iterator_scrub();

	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object);
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);
} ;

#endif /* DIRECTORY_ITERATOR_SCRUB_H_ */
