/*
	COMPRESSION_TEXT_FACTORY.C
	--------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include "compression_text_factory.h"
#include "compression_text_factory_scheme.h"
#include "compress_text_deflate.h"
#include "compress_text_bz2.h"
#include "compress_text_snappy.h"
#include "compress_text_none.h"

/*
	ANT_COMPRESSION_TEXT_FACTORY::ANT_COMPRESSION_TEXT_FACTORY()
	------------------------------------------------------------
	We have to do this at run-time because the compression objects might call code that
	isn't necessarily re-entrant from multiple threads.
*/
ANT_compression_text_factory::ANT_compression_text_factory()
{
scheme = new ANT_compression_text_factory_scheme[number_of_techniques = TERMINAL];
scheme[RAW].scheme_id = RAW;
scheme[RAW].scheme = new ANT_compress_text_none;
scheme[RAW].name = "none";

scheme[DEFLATE].scheme_id = DEFLATE;
scheme[DEFLATE].scheme = new ANT_compress_text_deflate;
scheme[DEFLATE].name = "deflate";

scheme[BZ2].scheme_id = BZ2;
scheme[BZ2].scheme = new ANT_compress_text_bz2;
scheme[BZ2].name = "BZ2";

scheme[SNAPPY].scheme_id = SNAPPY;
scheme[SNAPPY].scheme = new ANT_compress_text_snappy;
scheme[SNAPPY].name = "Snappy";

set_scheme(DEFLATE);
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
	ANT_COMPRESSION_TEXT_FACTORY::SET_SCHEME()
	------------------------------------------
*/
void ANT_compression_text_factory::set_scheme(unsigned long new_scheme)
{
long which;

scheme_to_use = (unsigned char)new_scheme;
current_scheme = NULL;

/*
	We do a linear search, but strictly this isn't (currently) necessary because scheme[new_scheme] is the answer
	unless we overflow in which case new_scheme > TERMINAL.  None the less, as they might be out of order, lets take a look
	afterall, this isn't going to happen very often
*/
for (which = 0; which < number_of_techniques; which++)
	if ((scheme[which].scheme_id == scheme_to_use) != 0)
		{
		current_scheme = scheme[which].scheme;
		return;
		}

exit(printf("ANT_compression_text_factory has been asked to use an unidentifiable scheme\n"));
}

/*
	ANT_COMPRESSION_TEXT_FACTORY::REPLICATE()
	-----------------------------------------
*/
ANT_compression_text_factory *ANT_compression_text_factory::replicate(void)
{
ANT_compression_text_factory *answer;

answer = new ANT_compression_text_factory;
answer->set_scheme(scheme_to_use);

return answer;
}

/*
	ANT_COMPRESSION_TEXT_FACTORY::SPACE_NEEDED_TO_COMPRESS()
	--------------------------------------------------------
*/
unsigned long ANT_compression_text_factory::space_needed_to_compress(unsigned long source_length)
{
return current_scheme->space_needed_to_compress(source_length) + 1;		// +1 because we prepend the algorithm key at the front
}

/*
	ANT_COMPRESSION_TEXT_FACTORY::COMPRESS()
	----------------------------------------
*/
char *ANT_compression_text_factory::compress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length)
{
unsigned char which;
unsigned long shortened_destination_length = *destination_length - 1;			// make space for the preamble byte

/*
	Compress using the preferred technique but if that fails then default to no compression;
*/
if (current_scheme->compress(destination + 1, &shortened_destination_length, source, source_length) != NULL)
	which = scheme_to_use;
else
	scheme[which = RAW].scheme->compress(destination + 1, &shortened_destination_length, source, source_length);

*destination_length = shortened_destination_length + 1;
*destination = which;
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
return scheme[(unsigned char)*source].scheme->decompress(destination, destination_length, source + 1, source_length - 1);
}

