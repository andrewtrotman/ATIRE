/*
	INSTREAM_BUFFER.H
	-----------------
*/
#ifndef INSTREAM_BUFFER_H_
#define INSTREAM_BUFFER_H_

#include "instream.h"

class ANT_semaphores;

/*
	class ANT_INSTREAM_BUFFER
	-------------------------
*/
class ANT_instream_buffer : public ANT_instream
{
protected:
	static const long buffer_size = 1024 * 1024 * 16;
	static long tid;

	struct background_read_params {
		unsigned char ***buffer;
		ANT_semaphores *sem, *sem2;
		ANT_instream *source;
		long long **read_result;
	} params;

protected:
	unsigned char *primary_buffer, *secondary_buffer;
	unsigned char **buffer_to_read_into, **buffer_to_read_from;
	long long *end_of_buffer, *end_of_second_buffer;
	long long position;
	long long position_of_end_of_buffer, position_of_end_of_second_buffer;

	static void *background_read(void *);
	ANT_semaphores *sem, *sem2;

public:
	ANT_instream_buffer(ANT_memory *memory, ANT_instream *source);
	virtual ~ANT_instream_buffer();

	virtual long long read(unsigned char *data, long long size);

} ;

#endif /* INSTREAM_BUFFER_H_ */
