/*
	INSTREAM_SCRUB.C
	----------------
*/
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
//printf("%sstart_upstream %lld\n", message, clock->start_timer());
}

/*
	ANT_INSTREAM_SCRUB::~ANT_INSTREAM_SCRUB()
	-----------------------------------------
*/
ANT_instream_scrub::~ANT_instream_scrub()
{
//printf("%send_upstream %lld\n", message, clock->start_timer());
//clock->print_time(message, wait_input_time, " input");
//clock->print_time(message, wait_output_time, " upstream");
//clock->print_time(message, process_time, " process");

delete clock;
delete source;
}

/*
	ANT_INSTREAM_SCRUB::READ()
	--------------------------
*/
long long ANT_instream_scrub::read(unsigned char *data, long long size)
{
static long long now = clock->start_timer();
//printf("%send_upstream %lld\n", message, now);
//wait_output_time += clock->stop_timer(now);
now = clock->start_timer();

//printf("%sstart_wait %lld\n", message, clock->start_timer());
long long got = source->read(data, size);
//printf("%send_wait %lld\n", message, clock->start_timer());
//printf("%send_input_wait %lld\n", message, clock->start_timer());
//wait_input_time += clock->stop_timer(now);

// don't scrub out invalid utf-8 quite yet, wait till we have the whole document
//now = clock->start_timer();
START;
ANT_directory_iterator_scrub::scrub(data, got, scrubbing);
END;
//process_time += clock->stop_timer(now);

//printf("%sstart_upstream %lld\n", message, clock->start_timer());

return got;
}
