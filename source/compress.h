/*
	COMPRESS.H
	----------
*/

#ifndef __COMPRESS_H__
#define __COMPRESS_H__

#include "fundamental_types.h"

/*
	class ANT_COMPRESS
	------------------
*/
class ANT_compress
{
protected:
	unsigned long length_of_longest_possible_list;

public:
	ANT_compress(unsigned long max_list_length) { length_of_longest_possible_list = max_list_length; }
	virtual ~ANT_compress() {}

	/*
		destination_length is in bytes.  source_integers is in units of uint32_t returns the length in bytes
	*/
	virtual long compress(unsigned char *destination, unsigned long destination_length, uint32_t *source, unsigned long source_integers) = 0;
	virtual void decompress(uint32_t *destination, unsigned char *source, unsigned long destination_integers) = 0;
} ;

#endif __COMPRESS_H__

