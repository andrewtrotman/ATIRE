/*
	FILE.H
	------
*/

#ifndef FILE_H_
#define FILE_H_

#include "fundamental_types.h"

class ANT_memory;
class ANT_file_internals;

/*
	class ANT_FILE
	--------------
*/
class ANT_file
{
private:
	ANT_file_internals *internals;
	long long buffer_size, buffer_used;
	long long bytes_written, bytes_read;

protected:
	long long file_position;
	unsigned char *buffer;
	ANT_memory *memory;

private:
	void flush(void);

public:
	ANT_file(ANT_memory *memory);
	virtual ~ANT_file();

	virtual long setvbuff(long size);
	virtual long open(char *filename, char *mode);
	virtual long close(void);
	virtual long write(unsigned char *data, long long size);
	virtual long long puts(char *string);
	virtual long read(unsigned char *data, long long size);
	long read(int64_t *data) { return read((unsigned char *)data, sizeof(*data)); }
	long read(int32_t *data) { return read((unsigned char *)data, sizeof(*data)); }
	long long tell(void) { return file_position; }
	virtual void seek(long long offset_from_start_of_file);
	virtual long long file_length(void);

	long long get_bytes_read(void) { return bytes_read; }
	long long get_bytes_written(void) { return bytes_written; }
} ;

#endif  /* FILE_H_ */
