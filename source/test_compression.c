/*
	TEST_COMPRESSION.C
	------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "compress_relative10.h"
#include "compress_carryover12.h"
#include "compress_golomb.h"
#include "compress_variable_byte.h"
#include "compression_factory.h"
#include "compress_sigma.h"

#define ITERATIONS 5
#define TEST_LENGTH 30

ANT_compressable_integer random_buffer[TEST_LENGTH] = 
/*
this list is 89 in length
{2,29870,0,1,3750,4813,159,171,558,2763,2561,437,3368,2981,993,3581,3533,829,772,497,2313,356,1306,3007,282,3973,288,1847,1364,30,17,
3641,1957,2503,254,14592,1128,2095,3483,747,4737,1732,538,1141,247,1752,82,884,363,1372,4097,422,1995,1382,2727,1322,185,1268,950,138,
8925,657,5346,3070,3666,5583,2763,1895,2034,3770,3242,613,632,5094,855,957,836,630,542,55,3900,2528,3912,619,1287,398,2277,1672,0} ;
*/
{ 1,76795,0} ;

unsigned char buffer[TEST_LENGTH * sizeof(long) * 4];		// * 4 so that it is easily big enough
unsigned char second_buffer[TEST_LENGTH * sizeof(long)];
ANT_compressable_integer decode_buffer[TEST_LENGTH];

/*
	MAIN()
	------
*/
int main(void)
{
ANT_compressable_integer *into;
long iteration, which;
long long bytes;
ANT_compress_simple9 compressor, decompressor;

srand((unsigned int)time(NULL));
srand(0);

for (iteration = 0; iteration < ITERATIONS; iteration++)
	{
	into = random_buffer;

	for (which = 0; which < TEST_LENGTH; which++)
		 *into++ = (rand() + 1) & 0xffffff;

	bytes = compressor.compress(buffer, sizeof(buffer), random_buffer, TEST_LENGTH);
	printf("Compressed from %lld to %lld bytes ", (long long)sizeof(random_buffer), bytes);
	memset(second_buffer, 0, sizeof(second_buffer));
	memcpy(second_buffer, buffer, (size_t)bytes);
	decompressor.decompress(decode_buffer, second_buffer, TEST_LENGTH);

	if (memcmp(random_buffer, decode_buffer, TEST_LENGTH * sizeof(*random_buffer)) == 0)
		printf("Iteration:%d (of %d) Success\n", iteration + 1, ITERATIONS);
	else
		{
		printf("Iteration:%d (of %d) Failure!\n", iteration + 1, ITERATIONS);
		for (which = 0; which < TEST_LENGTH; which++)
			if (random_buffer[which] == decode_buffer[which])
				printf("<%d->%d>", random_buffer[which], decode_buffer[which]);
			else
				printf("<%d->%d>", random_buffer[which], decode_buffer[which]);
		puts("");
		}
	}

return 0;
}

