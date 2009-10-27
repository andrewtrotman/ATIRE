/*
	COMPRESS_TEXT_DEFLATE.H
	-----------------------
*/
#ifndef COMPRESS_TEXT_DEFLATE_H_
#define COMPRESS_TEXT_DEFLATE_H_

#include "compress_text.h"

class ANT_compress_text_deflate_internals;

/*
	class ANT_COMPRESS_TEXT_DEFLATE
	-------------------------------
*/
class ANT_compress_text_deflate : public ANT_compress_text
{
private:
	ANT_compress_text_deflate_internals *internals;

public:
	ANT_compress_text_deflate();
	virtual ~ANT_compress_text_deflate();

	virtual char *compress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length);
	virtual char *decompress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length);
} ;


#endif /* COMPRESS_TEXT_DEFLATE_H_ */
