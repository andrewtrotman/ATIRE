/*
	FILE_MEMORY.H
	-------------
*/
#ifndef FILE_MEMORY_H_
#define FILE_MEMORY_H_

#include "pragma.h"
#include "file.h"

/*
	class ANT_FILE_MEMORY
	---------------------
*/
class ANT_file_memory : public ANT_file
{
private:
	long long length_of_file;

public:
	ANT_file_memory() : ANT_file() {}
	virtual ~ANT_file_memory() {}

#pragma ANT_PRAGMA_UNUSED_PARAMETER
	virtual long setvbuff(long size) { return 1; }
	virtual long open(const char *filename, char *mode);
	virtual long close(void) { return 1; }
	virtual long long file_length(void) { return length_of_file; }
	virtual long write(unsigned char *data, long long size);
	virtual long long puts(char *string);
	virtual long read(unsigned char *data, long long size);
	virtual unsigned char *read_return_ptr(unsigned char *data, long long size);
	using ANT_file::read;

	virtual void seek(long long offset_from_start_of_file);
} ;

#endif  /* FILE_MEMORY_H_ */
