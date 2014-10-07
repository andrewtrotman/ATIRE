/*
	INSTREAM_BZ2.H
	--------------
*/
#ifndef INSTREAM_BZ2_H_
#define INSTREAM_BZ2_H_

#include "instream.h"

class ANT_instream_bz2_internals;
class ANT_file;
class ANT_memory;

/*
	class ANT_INSTREAM_BZ2
	----------------------
*/
class ANT_instream_bz2 : public ANT_instream
{
private:
	ANT_instream_bz2_internals *internals;
	long long total_written;			// number of bytes returned
	long long total_read;				// number of bytes read from the source stream
	unsigned char *buffer;
	static const long long buffer_length = 1024 * 1024 * 16;			// did use a 16K buffer, now using 16MB buffer

public:
	ANT_instream_bz2(ANT_memory *memory, ANT_instream *source);
	virtual ~ANT_instream_bz2();

	virtual long long read(unsigned char *data, long long size);
} ;



#endif /* INSTREAM_BZ2_H_ */
