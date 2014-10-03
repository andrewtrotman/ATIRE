/*
	INSTREAM.H
	----------
*/
#ifndef INSTREAM_H_
#define INSTREAM_H_

#ifndef START
	#ifdef LOGGING
		#define START printf("%ss %lld\n", message, clock->start_timer())
		#define END printf("%se %lld\n", message, clock->start_timer())
	#else
		#define START //
		#define END //
	#endif
#endif

#include "stats.h"

class ANT_memory;

/*
	class ANT_INSTREAM
	------------------
*/
class ANT_instream
{
protected:
	ANT_memory *memory;
	ANT_instream *source;

public:
	ANT_instream(ANT_memory *memory, ANT_instream *source = 0) { this->memory = memory; this->source = source; }
	virtual ~ANT_instream() { delete source; }

	virtual long long read(unsigned char *buffer, long long bytes) = 0;
	virtual long long read(char *buffer, long long bytes) { return read((unsigned char *)buffer,bytes); }

	char *message;
	unsigned long long wait_input_time;
	unsigned long long wait_output_time;
	unsigned long long process_time;
	ANT_stats *clock;
	bool printed;
} ;


#endif /* INSTREAM_H_ */
