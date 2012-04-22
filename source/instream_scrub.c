/*
	INSTREAM_SCRUB.C
	----------------
*/
#include <stdio.h>
#include "directory_iterator_scrub.h"
#include "instream_scrub.h"

/*
	ANT_INSTREAM_SCRUB::ANT_INSTREAM_SCRUB()
	----------------------------------------
*/
ANT_instream_scrub::ANT_instream_scrub(ANT_memory *memory, ANT_instream *source, long long scrubbing) : ANT_instream(memory, source)
{
this->source = source;
this->scrubbing = scrubbing;
}

/*
	ANT_INSTREAM_SCRUB::READ()
	--------------------------
*/
long long ANT_instream_scrub::read(unsigned char *data, long long size)
{
long long got = source->read(data, size);

// don't scrub out invalid utf-8 quite yet, wait till we have the whole document
ANT_directory_iterator_scrub::scrub(data, got, scrubbing);

return got;
}
