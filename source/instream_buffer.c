/*
	INSTREAM_BUFFER.C
	-----------------
*/
#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "instream_buffer.h"

/*
	ANT_INSTREAM_BUFFER::ANT_INSTREAM_BUFFER()
	------------------------------------------
*/
ANT_instream_buffer::ANT_instream_buffer(ANT_memory *memory, ANT_instream *source) : ANT_instream(memory, source)
{
position_of_end_of_buffer = position = 0;
buffer = 0;
}

/*
	ANT_INSTREAM_BUFFER::~ANT_INSTREAM_BUFFER()
	-------------------------------------------
*/
ANT_instream_buffer::~ANT_instream_buffer()
{
}

/*
	ANT_INSTREAM_BUFFER::READ()
	---------------------------
*/
long long ANT_instream_buffer::read(unsigned char *data, long long size)
{
long long where, remainder;

if (buffer == NULL)
	buffer = (unsigned char *)memory->malloc(buffer_size);

if (position + size < position_of_end_of_buffer)
	{
	memcpy(data, buffer + position, (size_t)size);
	position += size;
	}
else
	{
	if (position_of_end_of_buffer < 0)
		return -1;			// at EOF

	where = position_of_end_of_buffer - position;
	remainder = size - where;
	memcpy(data, buffer + position, (size_t)where);

	do
		{
		if ((position_of_end_of_buffer = source->read(buffer, buffer_size)) <= 0)
			return where;		// at EOF

		if (remainder > position_of_end_of_buffer)
			{
			memcpy(data + where, buffer, (size_t)position_of_end_of_buffer);
			where += position_of_end_of_buffer;
			remainder -= position_of_end_of_buffer;
			position = position_of_end_of_buffer;
			}
		else
			{
			memcpy(data + where, buffer, (size_t)remainder);
			position = remainder;
			remainder = 0;
			}
		}
	while (remainder > 0);
	}

return size;
}
