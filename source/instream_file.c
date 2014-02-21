/*
	INSTREAM_FILE.C
	---------------
*/
#include <string.h>
#include "instream_file.h"
#include "memory.h"
#include "file.h"

long ANT_instream_file::tid = 0;

/*
	ANT_INSTREAM_FILE::ANT_INSTREAM_FILE()
	--------------------------------------
*/
ANT_instream_file::ANT_instream_file(ANT_memory *memory, char *filename) : ANT_instream(memory)
{
this->filename = (char *)memory->malloc(strlen(filename) + 1);
strcpy(this->filename, filename);
memory->realign();

wait_input_time = 0;
wait_output_time = 0;
process_time = 0;
clock = new ANT_stats(memory);

message = new char[50];
sprintf(message, "file %ld ", ANT_instream_file::tid++);
//printf("%sstart_upstream %lld\n", message, clock->start_timer());

file = NULL;
file_length = bytes_read = 0;
}

/*
	ANT_INSTREAM_FILE::~ANT_INSTREAM_FILE()
	---------------------------------------
*/
ANT_instream_file::~ANT_instream_file()
{
//printf("%send_upstream %lld\n", message, clock->start_timer());
//clock->print_time(message, wait_input_time, " input");
//clock->print_time(message, wait_output_time, " upstream");
//clock->print_time(message, process_time, " process");

delete file;
delete clock;
}

/*
	ANT_INSTREAM_FILE::READ()
	-------------------------
*/
long long ANT_instream_file::read(unsigned char *buffer, long long bytes)
{
//static long long now = clock->start_timer();
//wait_output_time += clock->stop_timer(now);
//now = clock->start_timer();

//printf("%send_upstream %lld\n", message, now);
START;

if (bytes == 0)		// read no bytes
	{
END;
//printf("%sstart_upstream %lld\n", message, clock->start_timer());
//	wait_input_time += clock->stop_timer(now);
	return 0;
	}

if (file == NULL)
	{
	file = new ANT_file;
	if ((file->open(filename, "rb")) == 0)
		file_length = 0;
	else
		file_length = file->file_length();
	bytes_read = 0;
	}
if (bytes_read >= file_length)
	{
END;
//printf("%sstart_upstream %lld\n", message, clock->start_timer());
//	wait_input_time += clock->stop_timer(now);
	return -1;		// at EOF so nothing to read
	}


if (bytes_read + bytes > file_length)
	bytes = file_length - bytes_read;

bytes_read += bytes;

//printf("%sstart_wait %lld\n", message, clock->start_timer());
file->read(buffer, bytes);
END;
//printf("%send_wait %lld\n", message, clock->start_timer());
//printf("%sstart_upstream %lld\n", message, clock->start_timer());
//wait_input_time += clock->stop_timer(now);

return bytes;
}

