/*
	COMPRESSION_TEXT_FACTORY.H
	--------------------------
*/
#ifndef COMPRESSION_TEXT_FACTORY_H_
#define COMPRESSION_TEXT_FACTORY_H_

#include "compression_text_factory_scheme.h"

/*
	class ANT_COMPRESSION_TEXT_FACTORY
	----------------------------------
*/
class ANT_compression_text_factory : public ANT_compress_text
{
public:
	enum {RAW = 1, DEFLATE = 2, BZ2 = 4} ;

private:
	long number_of_techniques;
	ANT_compression_text_factory_scheme *scheme;
	unsigned long schemes_to_use;
	
public:
	ANT_compression_text_factory();
	virtual ~ANT_compression_text_factory();

	virtual char *compress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length);
	virtual char *decompress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length);

	void set_scheme(unsigned long scheme) { schemes_to_use = scheme; }
	ANT_compression_text_factory *replicate(void);
} ;


#endif /* COMPRESSION_TEXT_FACTORY_H_ */
