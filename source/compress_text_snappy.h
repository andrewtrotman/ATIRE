/*
	COMPRESS_TEXT_SNAPPY.H
	----------------------
*/
#ifndef COMPRESS_TEXT_SNAPPY_H_
#define COMPRESS_TEXT_SNAPPY_H_

#include "compress_text.h"

class ANT_compress_text_snappy : public ANT_compress_text
{
public:
	ANT_compress_text_snappy() : ANT_compress_text () {}
	virtual ~ANT_compress_text_snappy() {}

	virtual char *compress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length);
	virtual char *decompress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length);
	virtual unsigned long space_needed_to_compress(unsigned long source_length);
} ;

#endif /* COMPRESS_TEXT_SNAPPY_H_ */
