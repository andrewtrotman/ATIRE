/*
	COMPRESSION_FACTORY.H
	---------------------
*/
#ifndef COMPRESSION_FACTORY_H_
#define COMPRESSION_FACTORY_H_

#include <string.h>
#include "compress.h"
#include "compression_factory_scheme.h"


#define ANT_COMPRESSION_FACTORY_END_PADDING 1024	// extra padding at the end because some schemes (Simple-9) don't know when to stop

/*
	class ANT_COMPRESSION_FACTORY
	-----------------------------
*/
class ANT_compression_factory : public ANT_compress
{
public:
	enum {	
		NONE 			=   1, 
		VARIABLE_BYTE 	=   2, 
		SIMPLE_9 		=   4, 
		RELATIVE_10 	=   8, 
		CARRYOVER_12 	=  16, 
		SIGMA 			=  32, 
		ELIAS_DELTA 	=  64, 
		ELIAS_GAMMA 	= 128, 
		GOLOMB 			= 256,
		SIMPLE_16		= 512,
		FOUR_INTEGER_VARIABLE_BYTE = 1024,
		SIMPLE_9_PACKED = 2048,
		SIMPLE_16_PACKED = 4096,
		SIMPLE_8B = 8192,
		SIMPLE_8B_PACKED = 16384
		} ;
private:
	static long number_of_techniques;
	static ANT_compression_factory_scheme scheme[];
	long long failures;
	long long compression_failures;
	long long decompression_overruns;
	long long decompression_mismatches;
	long long integers_compressed;
	unsigned long schemes_to_use;
	/*
		The validation buffer is used for checking the pre-compresson and decompressed compressed strings
		are the same, and also to measure the decompression performance.
	*/
	long validate;
	ANT_compressable_integer *validation_buffer;
	long long validation_buffer_length;
	
public:
	ANT_compression_factory();
	virtual ~ANT_compression_factory();

	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);

	void set_scheme(unsigned long scheme) { schemes_to_use = scheme; }
	void set_validation(long validate) { this->validate = validate; }
										
	void text_render(void);
} ;

#endif  /* COMPRESSION_FACTORY_H_ */
