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

/*
	ANT_INSTREAM_BUFFER::ANT_INSTREAM_BUFFER()
	------------------------------------------
*/
ANT_instream_buffer::ANT_instream_buffer(ANT_memory *memory, ANT_instream *source, long double_buffered) : ANT_instream(memory, source)
{
this->double_buffered = double_buffered;
position = 0;
primary_buffer = (unsigned char *)memory->malloc(buffer_size);

if (double_buffered)
	secondary_buffer = (unsigned char *)memory->malloc(buffer_size);

buffer_to_read_from = &primary_buffer;
end_of_buffer = &position_of_end_of_buffer;

if (double_buffered)
	{
	buffer_to_read_into = &secondary_buffer;
	end_of_second_buffer = &position_of_end_of_second_buffer;
	}
else
	{
	buffer_to_read_into = buffer_to_read_from;
	end_of_second_buffer = end_of_buffer;
	}

position_of_end_of_buffer = position_of_end_of_second_buffer = 0;

read_sem = new ANT_semaphores(1, 1);
swap_sem = new ANT_semaphores(0, 1);

if (double_buffered)
	{
	params.buffer = &buffer_to_read_into;
	params.read_sem = read_sem;
	params.swap_sem = swap_sem;
	params.source = source;
	params.read_result = &end_of_second_buffer;

	ANT_thread(background_read, (void *)&params); // start reading in the background
	}
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
unsigned char **tmp_buffer;
long long *tmp_result;

if (position + size < *end_of_buffer)
	{
	// all held within buffer, simply copy it across
	memcpy(data, *buffer_to_read_from + position, (size_t)size);
	position += size;
	}
else
	{
	if (*end_of_buffer < 0)
		return -1;			// at EOF

	// copy the end of the buffer -- the beginning of the requested data
	where = *end_of_buffer - position;
	remainder = size - where;
	memcpy(data, *buffer_to_read_from + position, (size_t)where);

	do
		{
		// fill the buffer up with more data from downstream

		// the background reader might still be reading if we've been fast
		// so wait for it to say it's safe to swap
		if (double_buffered)
			swap_sem->enter();

		// swap the primary/secondary buffers around
		tmp_buffer = buffer_to_read_into;
		buffer_to_read_into = buffer_to_read_from;
		buffer_to_read_from = tmp_buffer;

		// swap the pointers to the results around
		tmp_result = end_of_buffer;
		end_of_buffer = end_of_second_buffer;
		end_of_second_buffer = tmp_result;

		// tell the background reader to read into the swapped buffers
		if (double_buffered)
			read_sem->leave();

		if ((!double_buffered && (position_of_end_of_buffer = source->read(primary_buffer, buffer_size)) <= 0) || (double_buffered && *end_of_buffer <= 0))
			return where;		// at EOF

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

return size;
}
