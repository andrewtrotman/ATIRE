/*
	INSTREAM_BUFFER.H
	-----------------
*/
#ifndef INSTREAM_BUFFER_H_
#define INSTREAM_BUFFER_H_

#include "instream.h"

/*
	class ANT_INSTREAM_BUFFER
	-------------------------
*/
class ANT_instream_buffer : public ANT_instream
{
protected:
	static const long buffer_size = 1024 * 1024 * 16;

protected:
	unsigned char *buffer;
	long long position;
	long long position_of_end_of_buffer;

public:
	ANT_instream_buffer(ANT_memory *memory, ANT_instream *source);
	virtual ~ANT_instream_buffer();

	virtual long long read(unsigned char *data, long long size);

} ;

#endif /* INSTREAM_BUFFER_H_ */
