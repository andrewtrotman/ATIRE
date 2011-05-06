/*
	INSTREAM_LZO.H
	--------------
*/
#ifndef INSTREAM_LZO_H_
#define INSTREAM_LZO_H_

#include "instream.h"
class ANT_memory;

/*
	class ANT_INSTREAM_LZO
	----------------------
*/
class ANT_instream_lzo : public ANT_instream
{
private:
	unsigned long filter;				// the lzop filter to use
	unsigned long flags;				// the lzop flags in the LZO file
	long must_read_header;				// if TRUE then the first disk read must read the file header
	unsigned char *compressed_buffer;	// the most recent compressed block read from disk
	unsigned char *uncompressed_buffer;	// the current decompressed block
	unsigned char *current_uncompressed_position;	// current position in the uncompressed_buffer
	unsigned long compressed_size;		// size of the compressed block
	unsigned long uncompressed_size;	// size of the uncompressed block

protected:
	long check_lzo_header(void);
	long long decompress_next_block(void);
	long unfilter(unsigned char *buffer, unsigned long length, unsigned long filter);

	unsigned short shortize(unsigned char *source) { return ((unsigned short)source[0] << 8) | (unsigned short)source[1]; }
	unsigned short shortize(unsigned short *source) { return shortize((unsigned char *)source); }

	unsigned long longize(unsigned char *source) { return ((unsigned long)source[0] << 24) | ((unsigned long)source[1] << 16) + ((unsigned long)source[2] << 8) + (unsigned long)source[3]; }
	unsigned long longize(unsigned long *source) { return longize((unsigned char *)source); }

public:
	ANT_instream_lzo(ANT_memory *memory, ANT_instream *source);
	virtual ~ANT_instream_lzo() {};

	virtual long long read(unsigned char *data, long long size);
} ;

#endif /* INSTREAM_LZO_H_ */

