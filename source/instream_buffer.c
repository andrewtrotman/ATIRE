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
primary_buffer = (unsigned char *)memory->malloc(buffer_size);
secondary_buffer = (unsigned char *)memory->malloc(buffer_size);

buffer_to_read_from = &primary_buffer;
end_of_buffer = &position_of_end_of_buffer;

#ifdef DOUBLE_BUFFER
buffer_to_read_into = &secondary_buffer;
end_of_second_buffer = &position_of_end_of_second_buffer;
#else
buffer_to_read_into = buffer_to_read_from;
end_of_second_buffer = end_of_buffer;
#endif

position_of_end_of_buffer = source->read(primary_buffer, buffer_size);
position_of_end_of_second_buffer = 0;

wait_input_time = 0;
wait_output_time = 0;
process_time = 0;
clock = new ANT_stats(memory);

read_sem = new ANT_semaphores(1, 1);
swap_sem = new ANT_semaphores(0, 1);

message = new char[50];
sprintf(message, "buffer %ld ", ANT_instream_buffer::tid++);

#ifdef DOUBLE_BUFFER
params.buffer = &buffer_to_read_into;
params.read_sem = read_sem;
params.swap_sem = swap_sem;
params.source = source;
params.read_result = &end_of_second_buffer;

ANT_thread(background_read, (void *)&params); // start reading in the background
#endif
}

/*
	ANT_INSTREAM_BUFFER::~ANT_INSTREAM_BUFFER()
	-------------------------------------------
*/
ANT_instream_buffer::~ANT_instream_buffer()
{
}

/*
	ANT_INSTREAM_BUFFER::BACKGROUND_READ()
	--------------------------------------
*/
void *ANT_instream_buffer::background_read(void *params)
{
struct background_read_params *p = (struct background_read_params *)params;

for (;;)
	{
	p->read_sem->enter();
	**(p->read_result) = p->source->read(**p->buffer, buffer_size);
	p->swap_sem->leave();

	if (**(p->read_result) <= 0)
		break;
	}

return NULL;
}

/*
	ANT_INSTREAM_BUFFER::READ()
	---------------------------
*/
long long ANT_instream_buffer::read(unsigned char *data, long long size)
{
long long where, remainder;
START;

if (position + size < *end_of_buffer)
	{
	// all held within buffer, simply copy it across
	memcpy(data, *buffer_to_read_from + position, (size_t)size);
	position += size;
	}
else
	{
	if (*end_of_buffer < 0)
		{
		END;
		return -1;			// at EOF
		}

	// copy the end of the buffer -- the beginning of the requested data
	where = *end_of_buffer - position;
	remainder = size - where;
	memcpy(data, *buffer_to_read_from + position, (size_t)where);

	do
		{
		END;
		// fill the buffer up with more data from downstream

#ifdef DOUBLE_BUFFER
		// the background reader might still be reading if we've been fast
		// so wait for it to say it's safe to swap
		swap_sem->enter();
#endif

		// swap the primary/secondary buffers around
		unsigned char **tmp_buffer = buffer_to_read_into;
		buffer_to_read_into = buffer_to_read_from;
		buffer_to_read_from = tmp_buffer;

		// swap the pointers to the results around
		long long *tmp_result = end_of_buffer;
		end_of_buffer = end_of_second_buffer;
		end_of_second_buffer = tmp_result;

#ifdef DOUBLE_BUFFER
		// tell the background reader to read into the swapped buffers
		read_sem->leave();

		if (*end_of_buffer <= 0)
#else
		if ((position_of_end_of_buffer = source->read(primary_buffer, buffer_size)) <= 0)
#endif
			return where;		// at EOF

		START;
		// if what we have left to get is larger than buffer, copy it all across
		if (remainder > *end_of_buffer)
			{
			memcpy(data + where, *buffer_to_read_from, (size_t)*end_of_buffer);
			where += *end_of_buffer;
			remainder -= *end_of_buffer;
			position = *end_of_buffer;
			}
		else
			{
			// otherwise copy only what's needed
			memcpy(data + where, *buffer_to_read_from, (size_t)remainder);
			position = remainder;
			remainder = 0;
			}
		}
	while (remainder > 0);
	}

END;
return size;
}
