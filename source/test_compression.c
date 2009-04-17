/*
	TEST_COMPRESSION.C
	------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "compress_elias_gamma.h"

#define ITERATIONS 10
#define TEST_LENGTH (1024*1024)

uint32_t random_buffer[TEST_LENGTH];
unsigned char buffer[TEST_LENGTH * sizeof(long)];
unsigned char second_buffer[TEST_LENGTH * sizeof(long)];
uint32_t decode_buffer[TEST_LENGTH];

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
long bytes;

srand((unsigned int)time(NULL));

for (iteration = 0; iteration < ITERATIONS; iteration++)
	{
	into = random_buffer;
	for (which = 0; which < TEST_LENGTH; which++)
		 *into++ = rand();

	ANT_compress_elias_gamma compressor(TEST_LENGTH * 2);

	bytes = compressor.compress(buffer, sizeof(buffer), random_buffer, TEST_LENGTH);
	printf("Compressed from %lld to %lld bytes ", (long long)sizeof(random_buffer), (long long)bytes);
	memset(second_buffer, 0, sizeof(second_buffer));
	memcpy(second_buffer, buffer, bytes);
	compressor.decompress(decode_buffer, second_buffer, TEST_LENGTH);

	if (memcmp(random_buffer, decode_buffer, TEST_LENGTH * sizeof(*random_buffer)) == 0)
		printf("Iteration:%d (of %d) Success\n", iteration + 1, ITERATIONS);
	else
		printf("Iteration:%d (of %d) Failure!\n", iteration + 1, ITERATIONS);
	}

return 0;
}

