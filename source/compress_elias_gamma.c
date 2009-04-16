/*
	COMPRESS_ELIAS_GAMMA.C
	----------------------
*/

#include "compress_elias_gamma.h"

/*
	ANT_COMPRESS_ELIAS_GAMMA::ANT_COMPRESS_ELIAS_GAMMA()
	----------------------------------------------------
*/
ANT_compress_elias_gamma::ANT_compress_elias_gamma()
{
bitstream = new ANT_bitstream;
}

/*
	ANT_COMPRESS_ELIAS_GAMMA::~ANT_COMPRESS_ELIAS_GAMMA()
	-----------------------------------------------------
*/
ANT_compress_elias_gamma::~ANT_compress_elias_gamma()
{
delete bitstream;
}

