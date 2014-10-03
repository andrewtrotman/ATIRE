/*
	INSTREAM_SCRUB.C
	----------------
*/
#include <stdlib.h>
#include <stdio.h>
#include "directory_iterator_scrub.h"
#include "instream_scrub.h"

long ANT_instream_scrub::tid = 0;

/*
	ANT_INSTREAM_SCRUB::ANT_INSTREAM_SCRUB()
	----------------------------------------
*/
ANT_instream_scrub::ANT_instream_scrub(ANT_memory *memory, ANT_instream *source, long long scrubbing) : ANT_instream(memory, source)
{
this->scrubbing = scrubbing;

wait_input_time = 0;
wait_output_time = 0;
process_time = 0;
clock = new ANT_stats(memory);

message = new char[50];
sprintf(message, "scrub %ld ", ANT_instream_scrub::tid++);
}

/*
	ANT_INSTREAM_SCRUB::~ANT_INSTREAM_SCRUB()
	-----------------------------------------
*/
ANT_instream_scrub::~ANT_instream_scrub()
{
delete clock;
}

/*
	ANT_INSTREAM_SCRUB::READ()
	--------------------------
*/
long long ANT_instream_scrub::read(unsigned char *data, long long size)
{
static long long now = clock->start_timer();
now = clock->start_timer();

long long got = source->read(data, size);

START;
ANT_directory_iterator_scrub::scrub(data, got, scrubbing);
END;

return got;
}
