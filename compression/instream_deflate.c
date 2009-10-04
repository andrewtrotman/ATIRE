/*
	INSTREAM_DEFLATE.C
	------------------
*/
#include "zlib.h"
#include "instream_deflate.h"
#include "instream_deflate_internals.h"

/*
	ANT_INSTREAM_DEFLATE::ANT_INSTREAM_DEFLATE()
	--------------------------------------------
*/
ANT_instream_deflate::ANT_instream_deflate(ANT_memory *memory, ANT_instream *source) : ANT_instream(memory, source)
{
total_written = total_read = 0;
internals = new (memory) ANT_instream_deflate_internals;

internals->stream.zalloc = Z_NULL;
internals->stream.zfree = Z_NULL;
internals->stream.opaque = Z_NULL;
internals->stream.avail_in = 0;
internals->stream.next_in = Z_NULL;

buffer = NULL;
}

/*
	ANT_INSTREAM_DEFLATE::~ANT_INSTREAM_DEFLATE()
	---------------------------------------------
*/
ANT_instream_deflate::~ANT_instream_deflate()
{
inflateEnd(&internals->stream);
}

/*
	ANT_INSTREAM_DEFLATE::READ()
	----------------------------
*/
long long ANT_instream_deflate::read(unsigned char *data, long long size)
{
long long got;
long state;

if (size == 0)
	return 0;

if (buffer == NULL)
	{
	buffer = (unsigned char *)memory->malloc(buffer_length);
	//ret = inflateInit2(&internals->steeam, 40);
	if (inflateInit(&internals->stream) != Z_OK)
		return -1;		// error
	}

internals->stream.avail_out = size;
internals->stream.next_out = data;

do
	{
	if (internals->stream.avail_in <= 0)
		{
		if ((got = source->read(buffer, buffer_length)) < 0)
			return -1;			// the instream is at EOF and so we are too
		internals->stream.avail_in = got;
		internals->stream.next_in = buffer;	
		}

	state = inflate(&internals->stream, Z_NO_FLUSH);

	if (state == Z_STREAM_END)
		{
		got = size - internals->stream.avail_out;		// number of bytes that were decompressed
		total_written += got;
		return got;			// at EOF
		}

	if (internals->stream.avail_out == 0)
		{
		total_written += size;
		return size;			// filled the output buffer and so return bytes read
		}
	}
while (state == Z_OK);

return -1;			// something has gone wrong
}

