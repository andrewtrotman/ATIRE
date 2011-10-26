/*
	INSTREAM_LZO.C
	--------------
*/
#include <string.h>
#include "memory.h"
#include "instream_lzo.h"
#include "fundamental_types.h"
#ifdef ANT_HAS_LZO
	#include "lzoconf.h"
	#include "lzo1x.h"
#endif

#define ANT_COMPATIBLE_WITH_LZOP_VERSION 0x1030		/* the version of LZOP this code is compatible with */

#define LZOP_ADLER32_D     0x0001
#define LZOP_ADLER32_C     0x0002
#define LZOP_EXTRA_FIELD   0x0040
#define LZOP_CRC32_D       0x0100
#define LZOP_CRC32_C       0x0200
#define LZOP_MULTIPART     0x0400
#define LZOP_FILTER        0x0800

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_INSTREAM_LZO::ANT_INSTREAM_LZO()
	------------------------------------
*/
ANT_instream_lzo::ANT_instream_lzo(ANT_memory *memory, ANT_instream *source) : ANT_instream(memory, source)
{
#ifdef ANT_HAS_LZO
	flags = filter = 0;
	must_read_header = TRUE;
	compressed_size = uncompressed_size = 0;
	current_uncompressed_position = uncompressed_buffer = compressed_buffer = NULL;
	lzo_init();
#else
	exit(printf("You are trying to decompress an lzo file but LZO-lib is not included in this build"));
#endif
}

/*
	ANT_INSTREAM_LZO::UNFILTER()
	----------------------------
	returns "did we do anything"

	This code is highly unlikely to ever be called, because filters are for "multimedia" apps
	and ANT is for text.  Besides, I doubt the effect is significant, and I doubt any user
	would know how to use this lzop parameter.
*/
long ANT_instream_lzo::unfilter(unsigned char *buffer, unsigned long length, unsigned long filter)
{
unsigned char byte;
unsigned char bytes[16];
unsigned long which, current;

if (filter == 0)
	return FALSE;

if (filter == 1)
	{
	byte = 0;
	for (current = 0; current < length; current++)
		{
		byte += *buffer;
		*buffer++ = byte;
		}
	return TRUE;
	}

if (filter <= 16)
	{
	which = 0;
	memset(bytes, 0, sizeof(bytes));			// this should be inlined by the compiler
	for (current = 0; current < length; current++)
		{
		bytes[which] += *buffer;
		*buffer++ = bytes[which];
		which++;
		if (which >= filter)
			which = filter;
		}
	return TRUE;
	}

return FALSE;
}

/*
	ANT_INSTREAM_LZO::CHECK_LZO_HEADER()
	------------------------------------
*/
long ANT_instream_lzo::check_lzo_header(void)
{
#ifdef ANT_HAS_LZO
	static const unsigned char lzop_header[] = {0x89, 0x4c, 0x5a, 0x4f, 0x00, 0x0d, 0x0a, 0x1a, 0x0a};
	unsigned long version, number_of_extra_bytes, current;
	unsigned char one;
	uint16_t two;
	uint32_t four;
	unsigned char nine[9];
	unsigned char two_fifty_six[256];

	/*
		9 bytes LZO Header
	*/
	source->read(nine, sizeof(lzop_header));
	if (memcmp(nine, lzop_header, sizeof(lzop_header)) != 0)
		return FALSE;

	source->read((unsigned char *)&two, sizeof(two));			// LZOP version number (2 bytes)
	version = shortize(&two);
	source->read((unsigned char *)&two, sizeof(two));			// LZO lib version number (2 bytes)

	if (version >= 0x0940)
		{
		source->read((unsigned char *)&two, sizeof(two));		// version needed to extract (2 bytes)
		if (shortize(&two) > ANT_COMPATIBLE_WITH_LZOP_VERSION)
			return FALSE;
		}

	source->read(&one, sizeof(one));			// compression algorithm (1 byte)
	if (one > 3)			// we only support LZO1X-1, LZO1X-1(15), and LZO1X-999 (same as lzop)
		return FALSE;

	if (version >= 0x0940)
		source->read(&one, sizeof(one));		// compression level (1 byte)

	source->read((unsigned char *)&four, sizeof(four));			// flags (4 bytes)
	flags = longize((unsigned long *)&four);
	if (flags & LZOP_MULTIPART)		// we do not support multipart archives (nor does lzop)
		return FALSE;

	if (flags & LZOP_FILTER)
		{
		source->read((unsigned char *)&four, sizeof(four));		// filter (4 bytes)
		filter = longize((unsigned long *)&four);
		}

	source->read((unsigned char *)&four, sizeof(four));			// mode (4 bytes)
	source->read((unsigned char *)&four, sizeof(four));			// time (low) (4 bytes)
	if (version >= 0x0940)
		source->read((unsigned char *)&four, sizeof(four));		// time (high) (4 bytes)

	/*
		Now we get the name of the compressed file (as a Pascal string!)
	*/
	source->read(&one, sizeof(one));			// length, n (1 byte)
	source->read(two_fifty_six, one);			// filename (n bytes)

	source->read((unsigned char *)&four, sizeof(four));			// header checksum (4 bytes)

	/*
		Now we get an "extra field", but lzop doesn't use this so we're compatible
		with anything else that might.
	*/
	if (flags & LZOP_EXTRA_FIELD)
		{
		source->read((unsigned char *)&four, sizeof(four));			// length (4 bytes)
		number_of_extra_bytes = longize((unsigned long *)&four);
		for (current = 0; current < number_of_extra_bytes; current++)
			source->read(&one, sizeof(one));		// each byte (1 byte each)
		source->read((unsigned char *)&four, sizeof(four));			// checksum (4 bytes)
		}

	return TRUE;
#else
	return FALSE;
#endif

}

/*
	ANT_INSTREAM_LZO::DECOMPRESS_NEXT_BLOCK()
	-----------------------------------------
	decompresses a block from an LZO file and returns the number of bytes it decompressed (or 0 on error)
*/
long long ANT_instream_lzo::decompress_next_block(void)
{
#ifdef ANT_HAS_LZO
	uint32_t four;
	unsigned long worst_case;
	lzo_uint used;

	/*
		At start of disk file? If so then read all the cack at the start
	*/
	if (must_read_header)
		{
		must_read_header = FALSE;
		if (!check_lzo_header())
			return 0;					// failure to read the disk header
		}
	/*
		Compressed and uncompressed size (in bytes)
	*/
	source->read((unsigned char *)&four, sizeof(four));
	uncompressed_size = longize((unsigned long *)&four);
	if (uncompressed_size == 0)
		return 0;
	source->read((unsigned char *)&four, sizeof(four));
	compressed_size = longize((unsigned long *)&four);

	/*
		Uncompressed checksums
	*/
	if (flags & LZOP_ADLER32_D)
		source->read((unsigned char *)&four, sizeof(four));
	if (flags & LZOP_CRC32_D)
		source->read((unsigned char *)&four, sizeof(four));

	/*
		Compressed checksums
	*/
	if (flags & LZOP_ADLER32_C)
		source->read((unsigned char *)&four, sizeof(four));
	if (flags & LZOP_CRC32_C)
		source->read((unsigned char *)&four, sizeof(four));

	/*
		Allocate the buffer to store the compressed block.

		lzop, the LZO compressor breaks the source file into 256KB (128KB on DOS) blocks and compresses each one.
		If the raw data cannot be compressed then it becomes larger.  According to LZO.FAQ the largest you can get
		for LZO1 is (raw + (raw / 16) + 64 + 3), but for LZO2 it is (raw + (raw / 8) + 128 + 3).  So, at this point
		we know the raw (uncompressed) chunking and so we know the worst case for the size of the compresed block,
		consequently we can malloc both the compressed and uncompressed blocks.
	*/
	if (uncompressed_size == (unsigned long)0xffffffff)
		return 0;					// this is a "split" (what ever that means) file and lzop doesn't support them

	if (uncompressed_buffer == NULL)
		uncompressed_buffer = (unsigned char *)memory->malloc(uncompressed_size);
	if (compressed_buffer == NULL)
		{
		worst_case = uncompressed_size + (uncompressed_size / 8) + 128 + 3;
		compressed_buffer = (unsigned char *)memory->malloc(compressed_size > worst_case ? compressed_size : worst_case);
		}

	/*
		Read from disk
	*/
	source->read(compressed_buffer, compressed_size);

	/*
		Decompress
	*/
	used = uncompressed_size;
	if (lzo1x_decompress(compressed_buffer, compressed_size, uncompressed_buffer, &used, NULL) != LZO_E_OK)
		return 0;

	/*
		Unfilter
	*/
	if (flags & LZOP_FILTER)
		unfilter(uncompressed_buffer, uncompressed_size, filter);
	/*
		Rewind to the beginning of the decompressed block and then return the number of bytes that were decompressed
	*/
	current_uncompressed_position = uncompressed_buffer;

	return uncompressed_size;
#else
	return 0;
#endif
}

/*
	ANT_INSTREAM_LZO::READ()
	------------------------
*/
long long ANT_instream_lzo::read(unsigned char *data, long long size)
{
#ifdef ANT_HAS_LZO
	unsigned char *into;
	long long total;

	if (size == 0)
		return 0;

	into = data;
	total = 0;
	do
		{
		if (size <= uncompressed_size)
			{
			total += size;
			memcpy(into, current_uncompressed_position, (size_t)size);
			current_uncompressed_position += (size_t)size;
			uncompressed_size -= (unsigned long)size;

			return total;
			}

		memcpy(into, current_uncompressed_position, uncompressed_size);
		total += uncompressed_size;
		into += uncompressed_size;
		size -= uncompressed_size;
		}
	while (decompress_next_block() != 0);

	return total;			// something has gone wrong (or EOF)
#else
	return -1;
#endif
}

