/*
	BITSTREAM.C
	-----------
*/
#include <stdio.h>
#include "bitstream.h"

/*
	ANT_BITSTREAM::ANT_BITSTREAM()
	------------------------------
*/
ANT_bitstream::ANT_bitstream()
{
rewind();
}

/*
	ANT_BITSTREAM::REWIND()
	-----------------------
	it is assumed to be OK to convert destination into an integer type and to assign to it
*/
void ANT_bitstream::rewind(unsigned char *destination, long long destination_length)
{
stream = (uint32_t *)destination;
stream_length = destination_length / sizeof(uint32_t);		// convert into chunk sizes

failed = FALSE;
stream_pos = 0;
bit_pos = 0;
buffer = 0;
total_bits = 0;
}

/*
	ANT_BITSTREAM::EOF()
	--------------------
*/
long long ANT_bitstream::eof(void)
{
push_buffer();
return failed ? 0 : (total_bits + 7) / 8;			// return the number of bytes used or 0 on error
}

