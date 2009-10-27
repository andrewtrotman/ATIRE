/*
	COMPRESS_TEXT_BZ2.H
	-------------------
*/
#ifndef COMPRESS_TEXT_BZ2_H_
#define COMPRESS_TEXT_BZ2_H_

#include "compress_text.h"

class ANT_compress_text_bz2 : public ANT_compress_text
{
public:
	ANT_compress_text_bz2() : ANT_compress_text () {}
	virtual ~ANT_compress_text_bz2() {}

	virtual char *compress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length);
	virtual char *decompress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length);
} ;

#endif /* COMPRESS_TEXT_BZ2_H_ */
