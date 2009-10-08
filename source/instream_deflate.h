/*
	INSTREAM_DEFLATE.H
	------------------
*/
#ifndef INSTREAM_DEFLATE_H_
#define INSTREAM_DEFLATE_H_

#include "instream.h"

class ANT_instream_deflate_internals;
class ANT_file;
class ANT_memory;

/*
	class ANT_INSTREAM_DEFLATE
	--------------------------
*/
class ANT_instream_deflate : public ANT_instream
{
private:
	ANT_instream_deflate_internals *internals;
	long long total_written;			// number of bytes returned
	long long total_read;				// number of bytes read from the source stream
	unsigned char *buffer;
	static const long long buffer_length = (1024*1024);			// use a 1MB buffer

public:
	ANT_instream_deflate(ANT_memory *memory, ANT_instream *source);
	virtual ~ANT_instream_deflate();

	virtual long long read(unsigned char *data, long long size);
} ;

#endif /* INSTREAM_DEFLATE_H_ */
