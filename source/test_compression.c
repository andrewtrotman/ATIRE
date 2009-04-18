/*
	TEST_COMPRESSION.C
	------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "compress_golomb.h"

#define ITERATIONS 10
#define TEST_LENGTH (1024*1024)

ANT_compressable_integer random_buffer[TEST_LENGTH];
unsigned char buffer[TEST_LENGTH * sizeof(long)];
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
ANT_compress_golomb compressor(TEST_LENGTH), decompressor(TEST_LENGTH);

srand((unsigned int)time(NULL));

for (iteration = 0; iteration < ITERATIONS; iteration++)
	{
	into = random_buffer;
	for (which = 0; which < TEST_LENGTH; which++)
		 *into++ = rand();

	bytes = compressor.compress(buffer, sizeof(buffer), random_buffer, TEST_LENGTH);
	printf("Compressed from %lld to %lld bytes ", (long long)sizeof(random_buffer), bytes);
	memset(second_buffer, 0, sizeof(second_buffer));
	memcpy(second_buffer, buffer, (size_t)bytes);
	decompressor.decompress(decode_buffer, second_buffer, TEST_LENGTH);

	if (memcmp(random_buffer, decode_buffer, TEST_LENGTH * sizeof(*random_buffer)) == 0)
		printf("Iteration:%d (of %d) Success\n", iteration + 1, ITERATIONS);
	else
		printf("Iteration:%d (of %d) Failure!\n", iteration + 1, ITERATIONS);
	}

return 0;
}

