/*
	COMPRESSION_TEXT_FACTORY.C
	--------------------------
*/
#include <stdio.h>
#include "compression_text_factory.h"
#include "compression_text_factory_scheme.h"
#include "compress_text_deflate.h"
#include "compress_text_bz2.h"
#include "compress_text_none.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_COMPRESSION_TEXT_FACTORY::ANT_COMPRESSION_TEXT_FACTORY()
	------------------------------------------------------------
	We have to do this at run-time because the compression objects might call code that
	isn't necessarily re-entrant from multiple threads.
*/
ANT_compression_text_factory::ANT_compression_text_factory()
{
schemes_to_use = DEFLATE;

scheme = new ANT_compression_text_factory_scheme[number_of_techniques = 3];
scheme[0].scheme_id = 0;
scheme[0].scheme = new ANT_compress_text_none;
scheme[0].name = "none";

scheme[1].scheme_id = 1;
scheme[1].scheme = new ANT_compress_text_deflate;
scheme[1].name = "deflate";

scheme[2].scheme_id = 2;
scheme[2].scheme = new ANT_compress_text_bz2;
scheme[2].name = "BZ2";
}

/*
	ANT_COMPRESSION_TEXT_FACTORY::~ANT_COMPRESSION_TEXT_FACTORY()
	-------------------------------------------------------------
*/
ANT_compression_text_factory::~ANT_compression_text_factory()
{
long current;

for (current = 0; current < number_of_techniques; current++)
	 delete scheme[current].scheme;

delete [] scheme;
}

/*
	ANT_COMPRESSION_TEXT_FACTORY::REPLICATE()
	-----------------------------------------
*/
ANT_compression_text_factory *ANT_compression_text_factory::replicate(void)
{
ANT_compression_text_factory *answer;

answer = new ANT_compression_text_factory;
answer->set_scheme(schemes_to_use);

return answer;
}

/*
	ANT_COMPRESSION_TEXT_FACTORY::COMPRESS()
	----------------------------------------
*/
char *ANT_compression_text_factory::compress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length)
{
long which;
unsigned long shortened_destination_length = *destination_length - 1;			// make space for the preamble byte

for (which = 0; which < number_of_techniques; which++)
	if ((scheme[which].scheme_id & schemes_to_use) != 0)
		{
		/*
			Compress using the preferred technique
		*/
		if (scheme[which].scheme->compress(destination + 1, &shortened_destination_length, source, source_length) == NULL)
			{
			/*
				Compression failed which means we compress to larger than the input buffer so we resort to no compression
			*/
			scheme[which = RAW].scheme->compress(destination + 1, &shortened_destination_length, source, source_length);
			}
		break;
		}
*destination_length = shortened_destination_length;
*destination = (unsigned char)which;
return destination;
}

/*
	ANT_COMPRESSION_TEXT_FACTORY::DECOMPRESS()
	------------------------------------------
*/
char *ANT_compression_text_factory::decompress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length)
{
/*
	The first byte is a pre-amble byte that declares which compression strategy we are using
	so we use it to identify the decoder then remove it for decoding.
*/
return scheme[*source].scheme->decompress(destination, destination_length, source + 1, source_length - 1);
}

