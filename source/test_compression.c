/*
	TEST_COMPRESSION.C
	------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "compress_golomb.h"
#include "compress_elias_gamma.h"
#include "compress_elias_delta.h"

#define ITERATIONS 10
#define TEST_LENGTH (1024*1024)

unsigned long random_buffer[TEST_LENGTH];
unsigned long decode_buffer[TEST_LENGTH];

/*
	GOLOMB_FACTOR()
	---------------
*/
long golomb_factor(unsigned long *position, unsigned long count)
{
unsigned long *from, x, factor;
long long sum;

for (from = position, sum = x = 0; x < count; x++, from++)
	sum += *from;
if ((factor = (long)(0.69 * sum / ((long long)count))) < 1)
	factor = 1;

return factor;
}

/*
	MAIN()
	------
*/
int main(void)
{
unsigned long *into;
long iteration, which;

srand((unsigned int)time(NULL));

for (iteration = 0; iteration < ITERATIONS; iteration++)
	{
	into = random_buffer;
	for (which = 0; which < TEST_LENGTH; which++)
		 *into++ = rand();

	ANT_compress_golomb compressor(golomb_factor(random_buffer, TEST_LENGTH));
//	ANT_compress_elias_delta compressor;

	into = random_buffer;
	for (which = 0; which < TEST_LENGTH; which++)
		compressor.encode(*into++);
	compressor.eof();

	compressor.decode(decode_buffer, TEST_LENGTH);

	if (memcmp(random_buffer, decode_buffer, TEST_LENGTH * sizeof(*random_buffer)) == 0)
		printf("Iteration:%d (of %d) Success\n", iteration + 1, ITERATIONS);
	else
		printf("Iteration:%d (of %d) Failure!\n", iteration + 1, ITERATIONS);
	}

return 0;
}

