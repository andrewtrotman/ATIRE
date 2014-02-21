/*
	INSTREAM_BUFFER.C
	-----------------
*/
#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "instream_buffer.h"
#include "semaphores.h"
#include "threads.h"

long ANT_instream_buffer::tid = 0;

/*
	ANT_INSTREAM_BUFFER::ANT_INSTREAM_BUFFER()
	------------------------------------------
*/
ANT_instream_buffer::ANT_instream_buffer(ANT_memory *memory, ANT_instream *source) : ANT_instream(memory, source)
{
position = 0;
buffer = (unsigned char *)memory->malloc(buffer_size);
position_of_end_of_buffer = source->read(buffer, buffer_size);

wait_input_time = 0;
wait_output_time = 0;
process_time = 0;
clock = new ANT_stats(memory);

sem = new ANT_semaphores(0, 1);
sem->leave();

message = new char[50];
sprintf(message, "buffer %ld ", ANT_instream_buffer::tid++);

params.buffer = buffer;
params.sem = sem;

ANT_thread(background_read, (void *)&params);
}

/*
	ANT_INSTREAM_BUFFER::~ANT_INSTREAM_BUFFER()
	-------------------------------------------
*/
ANT_instream_buffer::~ANT_instream_buffer()
{
}

void *ANT_instream_buffer::background_read(void *params)
{
struct background_read_params *p = (struct background_read_params *)params;

p->sem->enter();
fprintf(stderr, "ANT_instream_buffer::background_read()\n");
fprintf(stderr, "%p %p\n", p->sem, p->buffer);
p->sem->leave();

return params;
}

/*
	ANT_INSTREAM_BUFFER::READ()
	---------------------------
*/
long long ANT_instream_buffer::read(unsigned char *data, long long size)
{
long long where, remainder;
START;

if (position + size < position_of_end_of_buffer)
	{
	// all held within buffer, simply copy it across
	memcpy(data, buffer + position, (size_t)size);
	position += size;
	}
else
	{
	if (position_of_end_of_buffer < 0)
		{
		END;
		return -1;			// at EOF
		}

	// copy the end of the buffer -- the beginning of the requested data
	where = position_of_end_of_buffer - position;
	remainder = size - where;
	memcpy(data, buffer + position, (size_t)where);

	do
		{
		END;
		// fill the buffer up with more data from downstream
		if ((position_of_end_of_buffer = source->read(buffer, buffer_size)) <= 0)
			return where;		// at EOF

		START;
		// if what we have left to get is larger than buffer, copy it all across
		if (remainder > position_of_end_of_buffer)
			{
			memcpy(data + where, buffer, (size_t)position_of_end_of_buffer);
			where += position_of_end_of_buffer;
			remainder -= position_of_end_of_buffer;
			position = position_of_end_of_buffer;
			}
		else
			{
			// otherwise copy only what's needed
			memcpy(data + where, buffer, (size_t)remainder);
			position = remainder;
			remainder = 0;
			}
		}
	while (remainder > 0);
	}

END;
return size;
}
