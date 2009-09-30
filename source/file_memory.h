/*
	FILE_MEMORY.H
	-------------
*/
#ifndef __FILE_MEMORY_H__
#define __FILE_MEMORY_H__

#include "file.h"

class ANT_file_memory : public ANT_file
{
private:
	long long length_of_file;

public:
	ANT_file_memory(ANT_memory *memory) : ANT_file(memory) {}

#pragma warning (suppress: 4100)
	virtual long setvbuff(long size) { return 1; }
	virtual long open(char *filename, char *mode);
	virtual long close(void) { return 1; }
	virtual long long file_length(void) { return length_of_file; }
	virtual long write(unsigned char *data, long long size);
	virtual long long puts(char *string);
	virtual long read(unsigned char *data, long long size);

	virtual void seek(long long offset_from_start_of_file);
} ;

#endif __FILE_MEMORY_H__
