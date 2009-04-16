/*
	COMPRESS_GOLOMB.C
	-----------------
*/
#include "compress_golomb.h"
#include "log2.h"

/*
	ANT_COMPRESS_GOLOMB::ANT_COMPRESS_GOLOMB()
	------------------------------------------
*/
ANT_compress_golomb::ANT_compress_golomb(long p_factor)
{
factor = p_factor;
bitstream = new ANT_bitstream;
log2_factor = ANT_log2(factor);
pivot = (1 << (log2_factor + 1)) - factor;
}

/*
	ANT_COMPRESS_GOLOMB::~ANT_COMPRESS_GOLOMB()
	-------------------------------------------
*/
ANT_compress_golomb::~ANT_compress_golomb()
{
delete bitstream;
}

