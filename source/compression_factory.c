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
#include "compress_simple16.h"
#include "compress_four_integer_variable_byte.h"
#include "compress_simple9_packed.h"
#include "compress_simple16_packed.h"
#include "compress_simple8b.h"
#include "compress_simple8b_packed.h"
#include "stats.h"

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
static ANT_compress_none none;
static ANT_compress_variable_byte variable_byte;
static ANT_compress_simple9 simple9;
static ANT_compress_relative10 relative10;
static ANT_compress_carryover12 carryover12;
static ANT_compress_elias_delta elias_delta;
static ANT_compress_elias_gamma elias_gamma;
static ANT_compress_golomb golomb;
static ANT_compress_sigma sigma;
static ANT_compress_simple16 simple16;
static ANT_compress_four_integer_variable_byte four_integer_variable_byte;
static ANT_compress_simple9_packed simple9_packed;
static ANT_compress_simple16_packed simple16_packed;
static ANT_compress_simple8b simple8b;
static ANT_compress_simple8b_packed simple8b_packed;

/*
	ANT_compression_factory::scheme[]
	---------------------------------
	The known compression schemes that can be used
*/
ANT_compression_factory_scheme ANT_compression_factory::scheme[] =
{
{NONE, &none, "No-Compression"},						// do not move this line - it is hard coded for being at poistion 0.
{VARIABLE_BYTE, &variable_byte, "Variable-Byte"},
{SIMPLE_9, &simple9, "Simple-9"},
{RELATIVE_10, &relative10, "Relative-10"},
{CARRYOVER_12, &carryover12, "Carryover-12"},
{SIGMA, &sigma, "Sigma"},
{ELIAS_DELTA, &elias_delta, "Elias-Delta"},
{ELIAS_GAMMA, &elias_gamma, "Elias-Gamma"},
{GOLOMB, &golomb, "Golomb"},
{SIMPLE_16, &simple16, "Simple-16"},
{FOUR_INTEGER_VARIABLE_BYTE, &four_integer_variable_byte, "Four Integer Variable Byte"},
{SIMPLE_9_PACKED, &simple9_packed, "Simple-9-Packed"},
{SIMPLE_16_PACKED, &simple16_packed, "Simple-16-Packed"},
{SIMPLE_8B, &simple8b, "Simple-8b"},
{SIMPLE_8B_PACKED, &simple8b_packed, "Simple-8b-Packed"}
};

long ANT_compression_factory::number_of_techniques = sizeof(ANT_compression_factory::scheme) / sizeof(*ANT_compression_factory::scheme);

/*
	ANT_COMPRESSION_FACTORY::ANT_COMPRESSION_FACTORY()
	--------------------------------------------------
*/
ANT_compression_factory::ANT_compression_factory()
{
long which;

/*
	By default use variable byte encoding
*/
schemes_to_use = VARIABLE_BYTE;

/*
	Now initialise everything to zero
*/
failures = integers_compressed = 0;
for (which = 0; which < number_of_techniques; which++)
	scheme[which].uses = scheme[which].would_take = scheme[which].did_take = scheme[which].did_compress = scheme[which].failures = scheme[which].time = 0;

/*
	Should we decompress and compare to the uncompressed string (and measure decompresson speed)
*/
validate = TRUE;
validation_buffer = NULL;
validation_buffer_length = -1;
}

/*
	ANT_COMPRESSION_FACTORY::~ANT_COMPRESSION_FACTORY()
	---------------------------------------------------
*/
ANT_compression_factory::~ANT_compression_factory()
{
delete [] validation_buffer;
}

/*
	ANT_COMPRESSION_FACTORY::COMPRESS()
	-----------------------------------
*/
long long ANT_compression_factory::compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers)
{
long which, preferred = -1;
long long min_size, size;
long long start_time;

min_size = LLONG_MAX;
integers_compressed += source_integers;

for (which = 0; which < number_of_techniques; which++)
	if ((scheme[which].scheme_id & schemes_to_use) != 0)
		{
		size = scheme[which].scheme->compress(destination + 1, destination_length - 1, source, source_integers);
		if (size == 0)				// failure
			scheme[which].failures++;
		else
			{
			scheme[which].would_take += size;
			if (size < min_size)		// if equal we prefer the first in the list
				{
				min_size = size;
				preferred = scheme[which].scheme_id;
				}
			}

		/*
			If selected, run the compression validator by decompressing each compressed string and
			comparing it to the original string.  While we're at it, also compute the decmpression
			rate so we can report integers per second decompression rates later.
		*/
		if (validate)
			{
			if (size == 0)
				printf("%s: Compression Failure (list length:%lld)\n", scheme[which].name, source_integers);
			else
				{
				if (validation_buffer_length < source_integers)
					{
					delete [] validation_buffer;		// efficiency isn't an issue (or else we wouldn't be validating)
					validation_buffer = new ANT_compressable_integer[(size_t)((validation_buffer_length = source_integers) + ANT_COMPRESSION_FACTORY_END_PADDING)];
					}
				validation_buffer[source_integers] = 0xCCCCCCCC;		// terminate the list so that we can check for overflow at the end

				start_time = ANT_stats::start_timer();		// time the decompression
				scheme[which].scheme->decompress(validation_buffer, destination + 1, source_integers);
				scheme[which].time += ANT_stats::stop_timer(start_time);

				if (memcmp(source, validation_buffer, (size_t)(source_integers * sizeof(ANT_compressable_integer))))
					printf("%s: Raw and decompressed strings do not match (list length:%lld, compressed-size:%lld)\n", scheme[which].name, source_integers, size);
				if (validation_buffer[source_integers] != 0xCCCCCCCC)
					printf("%s: Decompression Overrun (list length:%lld, compressed-size:%lld)\n", scheme[which].name, source_integers, size);
				}
			}
		}

if (preferred < 0)
	{
	failures++;
	preferred = ANT_compression_factory::NONE;		// in the case of failure to compress resort to storing the postings list on disk uncompressed.
	}

for (which = 0; which < number_of_techniques; which++)
	if (scheme[which].scheme_id == preferred)
		{
		*destination = (unsigned char)which;
		min_size = scheme[which].scheme->compress(destination + 1, destination_length - 1, source, source_integers);

		scheme[which].uses++;
		scheme[which].did_take += min_size;
		scheme[which].did_compress += source_integers;

		return min_size + 1;
		}

return 0;
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
	if ((scheme[which].scheme_id & schemes_to_use) != 0)
		{
		printf("%-*.*s :%10lld terms into %10lld bytes (%2.2f bpi)\n", 20, 20, scheme[which].name, scheme[which].uses, scheme[which].did_take, (double)(scheme[which].did_take * 8) / (double)scheme[which].did_compress);
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
for (which = 0; which < number_of_techniques; which++)
	if ((scheme[which].scheme_id & schemes_to_use) != 0)
		{
		printf("Only %-*.*s :%10lld bytes (%2.2f bpi)", 15, 15, scheme[which].name, scheme[which].would_take, (double)(scheme[which].would_take * 8) / (double)integers_compressed);
		if (scheme[which].failures != 0)
			printf(" Failed %10lld times ", scheme[which].failures);
		if (scheme[which].time != 0)
			printf(" Decompression Rate: %2.0f ips ", (double)integers_compressed / ((double)scheme[which].time / (double)ANT_stats::clock_tick_frequency()));
		printf("\n");
		if (scheme[which].would_take < best_other)
			best_other = scheme[which].would_take;
		}
printf("Mixed                :%10lld bytes (%2.2f bpi)\n", bytes + terms, (double)((bytes + terms) * 8) / (double)integers_compressed);
printf("Mixed cf best other  :%10lld bytes saved\n", best_other - (bytes + terms));
printf("Mixed cf best other  :%2.2f%% saved\n", 100.0 * ((double)(best_other - (bytes + terms)) / (double)best_other));
}

