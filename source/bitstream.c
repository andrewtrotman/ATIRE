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
stream = NULL;
stream_pos = stream_length = 0;
bit_pos = 0;
buffer = 0;
decomp_bit_pos = -1;
total_bits = 0;
}

/*
	ANT_BITSTREAM::~ANT_BITSTREAM()
	-------------------------------
*/
ANT_bitstream::~ANT_bitstream()
{
free(stream);
}

/*
	ANT_BITSTREAM::EOF()
	--------------------
*/
long ANT_bitstream::eof(void)
{
push_buffer();
return total_bits;
}

/*
	ANT_BITSTREAM::TEXT_RENDER()
	----------------------------
*/
void ANT_bitstream::text_render(void)
{
printf("Space: %d bytes %d bits", total_bits / 8, total_bits);
}

