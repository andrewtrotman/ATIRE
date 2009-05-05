/*
	COMPRESSION_FACTORY.C
	---------------------
*/
#include <stdio.h>
#include <limits.h>
#include "compression_factory.h"
#include "compression_factory_scheme.h"
#include "compress_relative10.h"
#include "compress_carryover12.h"
#include "compress_golomb.h"
#include "compress_variable_byte.h"
#include "compress_none.h"

/*
	static
	------
	Declaration of an object of each of the known compression types,
	pointers to these are used below as the objects to do the compression
	and decompression on demand.
*/
static ANT_compress_none none;
static ANT_compress_variable_byte variable_byte;
static ANT_compress_simple9 simple9;
static ANT_compress_relative10 relative10;
static ANT_compress_carryover12 carryover12;
static ANT_compress_elias_delta elias_delta;
static ANT_compress_elias_gamma elias_gamma;
static ANT_compress_golomb golomb;

ANT_compression_factory_scheme scheme[] =
{
{&none, "No-Compression", 0},
{&variable_byte, "Variable-Byte", 0},
{&simple9, "Simple-9", 0},
{&relative10, "Relative-10", 0},
{&carryover12, "Carryover-12", 0},
{&elias_delta, "Elias-Delta", 0},
{&elias_gamma, "Elias-Gamma", 0},
{&golomb, "Golomb", 0}
};

#ifdef NEVER
/*
	ANT_compression_factory::techniques[]
	-------------------------------------
	List of all known (to ANT) compression schemes
*/
ANT_compress *ANT_compression_factory::technique[] = 
{
&none,				/* type 0 */
&variable_byte,	/* type 1 */
&simple9,			/* type 2 */
&relative10,		/* type 3 */
&carryover12,		/* type 4 */
&elias_delta,		/* type 5 */
&elias_gamma,		/* type 6 */
&golomb			/* type 7 */
} ;

/*
	ANT_compression_factory::names[]
*/
char *ANT_compression_factory::technique_name[] =
{
"No-Compression",
"Variable-Byte",
"Simple-9",
"Relative-10",
"Carryover-12",
"Elias-Delta",
"Elias-Gamma",
"Golomb"
} ;

long ANT_compression_factory::number_of_techniques = sizeof(ANT_compression_factory::technique) / sizeof(*ANT_compression_factory::technique);

#endif

long ANT_compression_factory::number_of_techniques = sizeof(scheme) / sizeof(*scheme);


/*
	ANT_COMPRESSION_FACTORY::ANT_COMPRESSION_FACTORY()
	--------------------------------------------------
*/
ANT_compression_factory::ANT_compression_factory()
{
long which;

for (which = 0; which < number_of_techniques; which++)
	scheme[which].uses = scheme[which].times = scheme[which].bytes = 0;
}

/*
	ANT_COMPRESSION_FACTORY::COMPRESS()
	-----------------------------------
*/
long long ANT_compression_factory::compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers)
{
long which, preferred = -1;
long long min_size, size;

min_size = LLONG_MAX;

for (which = 0; which < number_of_techniques; which++)
	{
	size = scheme[which].scheme->compress(destination, destination_length, source, source_integers);

#ifdef ANT_COMPRESS_EXPERIMENT
	static ANT_compressable_integer d2[200000];
	scheme[which].scheme->decompress(d2, destination, source_integers);
	if (memcmp(source, d2, source_integers * sizeof(ANT_compressable_integer)))
		printf("%s: Raw and decompressed strings do not match (list length:%lld)\n", scheme[which].name, source_integers);
#endif

	if (size != 0 && size < min_size)		// if equal we prefer the first in the list
		{
		min_size = size;
		preferred = which;
		}
	}


if (preferred < 0)
	{
	/*
		failed to compress into the given space
		note that we do not keep track of the fact that we failed.
	*/
	return 0;
	}

scheme[preferred].uses++;

*destination = (unsigned char)preferred;
return scheme[preferred].scheme->compress(destination + 1, destination_length - 1, source, source_integers) + 1;
}

/*
	ANT_COMPRESSION_FACTORY::DECOMPRESS()
	-------------------------------------
*/
void ANT_compression_factory::decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers)
{
scheme[*source].scheme->decompress(destination, source + 1, destination_integers);
}

/*
	ANT_COMPRESSION_FACTORY::TEXT_RENDER()
	--------------------------------------
*/
void ANT_compression_factory::text_render(void)
{
long which;
long long terms;

puts("\nCOMPRESSION FACTORY USAGE");
puts("-------------------------");

terms = 0;
for (which = 0; which < number_of_techniques; which++)
	if (scheme[which].uses != 0)
		{
		printf("%-*.*s :%10lld terms\n", 20, 20, scheme[which].name, scheme[which].uses);
		terms += scheme[which].uses;
		}

printf("Factory calls        :%10lld terms\n", terms);
}

