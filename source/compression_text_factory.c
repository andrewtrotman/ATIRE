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
	static
	------
	Declaration of an object of each of the known compression types,
	pointers to these are used below as the objects to do the compression
	and decompression on demand.
*/
static ANT_compress_text_none none;
static ANT_compress_text_bz2 bz2;
static ANT_compress_text_deflate deflate;

/*
	ANT_compression_text_factory::scheme[]
	--------------------------------------
	The known compression schemes that can be used
*/
ANT_compression_text_factory_scheme ANT_compression_text_factory::scheme[] =
{
{NONE, &none, "none"},				// this must be in position zero as it is the fallback for the faulure case
{DEFLATE, &deflate, "deflate"},
{BZ2, &bz2, "BZ2"}
};

long ANT_compression_text_factory::number_of_techniques = sizeof(ANT_compression_text_factory::scheme) / sizeof(*ANT_compression_text_factory::scheme);

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
			scheme[which = NONE].scheme->compress(destination + 1, &shortened_destination_length, source, source_length);
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

