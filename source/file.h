/*
	FILE.H
	------
*/

#ifndef FILE_H_
#define FILE_H_

#include "fundamental_types.h"

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

private:
	void flush(void);

public:
	ANT_file(void);
	virtual ~ANT_file();

	virtual long setvbuff(long size);
	virtual long open(const char *filename, char *mode);
	virtual long close(void);
	virtual long write(unsigned char *data, long long size);
	long write(char *data, long long size) { return write((unsigned char *)data, size); }
	virtual long long puts(const char *string);

	virtual long read(unsigned char *data, long long size);
	long read(char *data, long long size) { return read((unsigned char *)data, size); }
	long read(int64_t *data) { return read((unsigned char *)data, sizeof(*data)); }
	long read(uint64_t *data) { return read((unsigned char *)data, sizeof(*data)); }
	long read(int32_t *data) { return read((unsigned char *)data, sizeof(*data)); }
	long read(uint32_t *data) { return read((unsigned char *)data, sizeof(*data)); }
	long read(int16_t *data) { return read((unsigned char *)data, sizeof(*data)); }
	long read(uint16_t *data) { return read((unsigned char *)data, sizeof(*data)); }
	long read(int8_t *data) { return read((unsigned char *)data, sizeof(*data)); }
	long read(uint8_t *data) { return read((unsigned char *)data, sizeof(*data)); }

	virtual unsigned char *read_return_ptr(unsigned char *data, long long size);
	unsigned char *read_return_ptr(char *data, long long size) { return read_return_ptr((unsigned char *)data, size); }
	unsigned char *read_return_ptr(int64_t *data) { return read_return_ptr((unsigned char *)data, sizeof(*data)); }
	unsigned char *read_return_ptr(uint64_t *data) { return read_return_ptr((unsigned char *)data, sizeof(*data)); }
	unsigned char *read_return_ptr(int32_t *data) { return read_return_ptr((unsigned char *)data, sizeof(*data)); }
	unsigned char *read_return_ptr(uint32_t *data) { return read_return_ptr((unsigned char *)data, sizeof(*data)); }
	unsigned char *read_return_ptr(int16_t *data) { return read_return_ptr((unsigned char *)data, sizeof(*data)); }
	unsigned char *read_return_ptr(uint16_t *data) { return read_return_ptr((unsigned char *)data, sizeof(*data)); }
	unsigned char *read_return_ptr(int8_t *data) { return read_return_ptr((unsigned char *)data, sizeof(*data)); }
	unsigned char *read_return_ptr(uint8_t *data) { return read_return_ptr((unsigned char *)data, sizeof(*data)); }

	virtual long long tell(void) { return file_position; }
	virtual void seek(long long offset_from_start_of_file);
	virtual long long file_length(void);

	long long get_bytes_read(void) { return bytes_read; }
	long long get_bytes_written(void) { return bytes_written; }
} ;

#endif  /* FILE_H_ */
