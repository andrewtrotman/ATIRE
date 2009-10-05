/*
	DIRECTORY_ITERATOR_TAR.H
	------------------------
*/
#ifndef DIRECTORY_ITERATOR_TAR_H_
#define DIRECTORY_ITERATOR_TAR_H_

#include "directory_iterator.h"
#include "directory_iterator_tar_file_header.h"
#include "instream.h"

/*
	class ANT_STREAM_TAR
	--------------------
*/
class ANT_directory_iterator_tar : public ANT_directory_iterator
{
private:
	static const long TAR_BLOCK_SIZE = 512;

private:
	ANT_instream *source;
	ANT_directory_iterator_tar_file_header header;
	unsigned char buffer[512];
	long long length_of_file_in_bytes;
	long long bytes_read;

private:
	long detox(char *octal);
	char *filename(void);

public:
	ANT_directory_iterator_tar(ANT_instream *source) { this->source = source; }
	virtual ~ANT_directory_iterator_tar() {}

	virtual char *first(char *wildcard);
	virtual char *next(void);
	virtual char *read_entire_file(long long *len = 0);

/*
	unsigned char *read(unsigned char *buffer, long long size) { bytes_read += size; return file_read(buffer, size); }
	long long filelength(void) { return length_of_file_in_bytes; }
	char filetype(void) { return header.file_type; }		// '0' = file '5' = directory
*/
} ;

#endif /* DIRECTORY_ITERATOR_TAR_H_ */
