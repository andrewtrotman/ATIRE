/*
	COMPRESSION_FACTORY.C
	---------------------
*/
#include <stdio.h>
#include <limits.h>
#include "compression_factory.h"
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
	size = technique[which]->compress(destination, destination_length, source, source_integers);

#ifdef ANT_COMPRESS_EXPERIMENT
	static ANT_compressable_integer d2[200000];
	technique[which]->decompress(d2, destination, source_integers);
	if (memcmp(source, d2, source_integers * sizeof(ANT_compressable_integer)))
		printf("%s: Raw and decompressed strings do not match (list length:%lld)\n", technique_name[which], source_integers);
#endif

	if (size != 0 && size < min_size)		// if equal we prefer the first in the list
		{
		min_size = size;
		preferred = which;
		}
	}

histogram[preferred]++;		// OK, this is a hack, histogram[-1] is the "cannot compress" count.

if (preferred < 0)
	return 0;			// fail, all compression schemes make the string longer!

*destination = (unsigned char)preferred;
return technique[preferred]->compress(destination + 1, destination_length - 1, source, source_integers) + 1;
}

/*
	ANT_COMPRESSION_FACTORY::DECOMPRESS()
	-------------------------------------
*/
void ANT_compression_factory::decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers)
{
technique[*source]->decompress(destination, source + 1, destination_integers);
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

terms = histogram[-1];
if (histogram[-1] != 0)
	printf("FAILED TO COMPRESS:%ld\n", histogram[-1]);
for (which = 0; which < number_of_techniques; which++)
	if (histogram[which] != 0)
		{
		printf("%-*.*s :%10lld terms\n", 20, 20, technique_name[which], histogram[which]);
		terms += histogram[which];
		}

printf("Factory calls        :%10lld terms\n", terms);
}

