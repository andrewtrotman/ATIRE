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
	enum {NONE, DEFLATE, BZ2, SNAPPY, RAW, TERMINAL};		// TERMINAL is a sentinal and must not be used

private:
	ANT_compression_text_factory_scheme *scheme;			// array of all possible schemes
	long number_of_techniques;								// the number of schemes we know about
	unsigned char scheme_to_use;							// the ID of the scheme to use (from the enum)
	ANT_compress_text *current_scheme;						// a pointer to the scheme we are currently using
	
public:
	ANT_compression_text_factory();
	virtual ~ANT_compression_text_factory();

	virtual char *compress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length);
	virtual char *decompress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length);

	virtual unsigned long space_needed_to_compress(unsigned long source_length);

	void set_scheme(unsigned long scheme);
	ANT_compression_text_factory *replicate(void);
} ;


#endif /* COMPRESSION_TEXT_FACTORY_H_ */
