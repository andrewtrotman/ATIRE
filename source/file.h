/*
	FILE.H
	------
*/

#ifndef __FILE_H__
#define __FILE_H__

class ANT_memory;
class ANT_file_internals;

class ANT_file
{
private:
	ANT_file_internals *internals;
	long long file_position;
	unsigned char *buffer;
	long long buffer_size, buffer_used;
	ANT_memory *memory;

private:
	void flush(void);

public:
	ANT_file(ANT_memory *memory);
	~ANT_file();

	long setvbuff(long size);
	long open(char *filename, char *mode);
	long close(void);
	long write(unsigned char *data, long long size);
	long long puts(char *string);
	long read(unsigned char *data, long size);
	long read(long long *data) { return read((unsigned char *)data, sizeof(long long)); }
	long read(long *data) { return read((unsigned char *)data, sizeof(long)); }
	long long tell(void);
	void seek(long long offset_from_start_of_file);
	long long file_length(void);
} ;



#endif __FILE_H__
