/*
	COMPRESSION_FACTORY.C
	---------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "compression_factory.h"
#include "compression_factory_scheme.h"
#include "compress_relative10.h"
#include "compress_carryover12.h"
#include "compress_golomb.h"
#include "compress_variable_byte.h"
#include "compress_sigma.h"
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
static ANT_compress_sigma sigma;

/*
	ANT_compression_factory::scheme[]
	---------------------------------
	The known compression schemes that can be used
*/
ANT_compression_factory_scheme ANT_compression_factory::scheme[] =
{
{&none, "No-Compression"},						// do not move this line - it is hard coded for being at poistion 0.
{&variable_byte, "Variable-Byte"},
{&simple9, "Simple-9"},
{&relative10, "Relative-10"},
{&carryover12, "Carryover-12"}
//{&sigma, "Sigma"}
//{&elias_delta, "Elias-Delta"},
//{&elias_gamma, "Elias-Gamma"},
//{&golomb, "Golomb"},
//{&none, "No-Compression"},
};

long ANT_compression_factory::number_of_techniques = sizeof(ANT_compression_factory::scheme) / sizeof(*ANT_compression_factory::scheme);

/*
	ANT_COMPRESSION_FACTORY::ANT_COMPRESSION_FACTORY()
	--------------------------------------------------
*/
ANT_compression_factory::ANT_compression_factory()
{
long which;

failures = integers_compressed = 0;
for (which = 0; which < number_of_techniques; which++)
	scheme[which].uses = scheme[which].would_take = scheme[which].did_take = scheme[which].failures = 0;
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
integers_compressed += source_integers;

for (which = 1; which < number_of_techniques; which++)
	{
	size = scheme[which].scheme->compress(destination, destination_length, source, source_integers);
	scheme[which].would_take += size;
	if (size == 0)				// failure
		scheme[which].failures++;

#ifdef NEVER
	/*
		This piece of code decompressed each posting during indexing to validate that compression was successful
	*/
	if (size == 0)
		printf("%s: Compression Failure\n", scheme[which].name);
	else
		{
		ANT_compressable_integer *d2;
		d2 = new ANT_compressable_integer[(size_t)(source_integers + 1)];
		d2[source_integers] = 0xCCCCCCCC;		// terminate the list so that we can check for overflow at the end
		scheme[which].scheme->decompress(d2, destination, source_integers);
		if (memcmp(source, d2, (size_t)(source_integers * sizeof(ANT_compressable_integer))))
			{
			printf("%s: Raw and decompressed strings do not match (list length:%lld, compressed-size:%lld)\n", scheme[which].name, source_integers, size);
			for (long err = 0; err < (source_integers < 10 ? source_integers : 10); err++)
				printf("[%lld->%lld]", (long long)source[err], (long long)d2[err]);
			printf("\n\n");
			}
		if (d2[source_integers] != 0xCCCCCCCC)
			printf("%s: Decompression overrun\n", scheme[which].name);
		delete [] d2;
		}
#endif

	if (size != 0 && size < min_size)		// if equal we prefer the first in the list
		{
		min_size = size;
		preferred = which;
		}
	}

if (preferred < 0)
	{
	failures++;
	return 0;
	}

scheme[preferred].uses++;
scheme[preferred].did_take += min_size;
scheme[preferred].did_compress += source_integers;

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
long long terms, bytes, best_other;

puts("\nCOMPRESSION FACTORY USAGE");
puts("-------------------------");

terms = bytes = 0;
for (which = 0; which < number_of_techniques; which++)
	if (scheme[which].uses != 0)
		{
		printf("%-*.*s :%10lld terms into %10lld bytes (%2.2f bits per integer)\n", 20, 20, scheme[which].name, scheme[which].uses, scheme[which].did_take, (double)(scheme[which].did_take * 8) / (double)scheme[which].did_compress);
		terms += scheme[which].uses;
		bytes += scheme[which].did_take;
		}
if (failures != 0)
	printf("*failures*           :%10lld calls\n", failures);
printf("Factory calls        :%10lld terms\n", terms);
printf("        into         :%10lld bytes\n", bytes);
printf("        total space  :%10lld bytes\n", bytes + terms);		// add terms because it takes one byte to store which scheme was used

best_other = LLONG_MAX;
puts("\nCOMPRESSION COMPARISON");
puts("----------------------");
for (which = 1; which < number_of_techniques; which++)
	{
	printf("Only %-*.*s :%10lld bytes (%2.2f bits per integer)", 15, 15, scheme[which].name, scheme[which].would_take, (double)(scheme[which].would_take * 8) / (double)integers_compressed);
	if (scheme[which].failures != 0)
		printf(" Failed %10lld times", scheme[which].failures);
	printf("\n");
	if (scheme[which].would_take < best_other)
		best_other = scheme[which].would_take;
	}
printf("Mixed                :%10lld bytes (%2.2f bits per integer)\n", bytes + terms, (double)((bytes + terms) * 8) / (double)integers_compressed);
printf("Mixed cf best other  :%10lld bytes saved\n", best_other - (bytes + terms));
printf("Mixed cf best other  :%2.2f%% saved\n", 100.0 * ((double)(best_other - (bytes + terms)) / (double)best_other));
}

