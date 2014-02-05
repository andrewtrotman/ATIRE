/*
	INSTREAM_BUFFER.C
	-----------------
*/
#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "instream_buffer.h"

long ANT_instream_buffer::tid = 0;

/*
	ANT_INSTREAM_BUFFER::ANT_INSTREAM_BUFFER()
	------------------------------------------
*/
ANT_instream_buffer::ANT_instream_buffer(ANT_memory *memory, ANT_instream *source) : ANT_instream(memory, source)
{
position_of_end_of_buffer = position = 0;
buffer = 0;

wait_input_time = 0;
wait_output_time = 0;
process_time = 0;
clock = new ANT_stats(memory);

message = new char[50];
sprintf(message, "ANT_instream_buffer %ld ", ANT_instream_buffer::tid++);
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
printf("%sstart_process %lld\n", message, clock->start_timer());

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
	{

printf("%send_process %lld\n", message, clock->start_timer());
		return -1;			// at EOF
	}

	where = position_of_end_of_buffer - position;
	remainder = size - where;
	memcpy(data, buffer + position, (size_t)where);

	do
		{
printf("%send_process %lld\n", message, clock->start_timer());
		if ((position_of_end_of_buffer = source->read(buffer, buffer_size)) <= 0)
			return where;		// at EOF

printf("%sstart_process %lld\n", message, clock->start_timer());
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

printf("%send_process %lld\n", message, clock->start_timer());
return size;
}
