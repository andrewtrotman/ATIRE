/*
	STREAM_TAR.H
	------------
*/
#ifndef STREAM_TAR_H_
#define STREAM_TAR_H_

#include "stream_tar_file_header.h"

/*
	class ANT_STREAM_TAR
	--------------------
*/
class ANT_stream_tar
{
private:
	static const long TAR_BLOCK_SIZE = 512;

private:
	FILE *fp;
	long long length_of_file_in_bytes;
	ANT_stream_tar_file_header header;
	unsigned char buffer[512];
	long long bytes_read;

private:
	long detox(char *octal);

protected:
	virtual long file_open(char *filename);
	virtual long file_close(void);
	virtual unsigned char *file_read(unsigned char *buffer, long long size);

public:
	long long first(char *filename);		// returns the length of the file (or -1)
	long long next(void);					// returns the length of the file (or -1)

	unsigned char *read(unsigned char *buffer, long long size) { bytes_read += size; return file_read(buffer, size); }
	char *filename(void);
	long long filelength(void) { return length_of_file_in_bytes; }
	char filetype(void) { return header.file_type; }		// '0' = file '5' = directory
} ;

#endif /* STREAM_TAR_H_ */
