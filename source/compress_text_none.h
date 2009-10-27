/*
	COMPRESS_TEXT_NONE.H
	--------------------
*/
#ifndef COMPRESS_TEXT_NONE_H_
#define COMPRESS_TEXT_NONE_H_

#include "compress_text.h"
/*
	class ANT_COMPRESS_TEXT_NONE
	----------------------------
*/
class ANT_compress_text_none : public ANT_compress_text
{
public:
	ANT_compress_text_none() {}
	virtual ~ANT_compress_text_none() {}

	virtual char *compress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length);
	virtual char *decompress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length);
} ;


#endif /* COMPRESS_TEXT_NONE_H_ */
