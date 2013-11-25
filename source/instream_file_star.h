/*
	INSTREAM_FILE_STAR.H
	--------------------
*/
#ifndef INSTREAM_FILE_STAR_H_
#define INSTREAM_FILE_STAR_H_

#include <stdio.h>
#include "instream.h"

/*
	class ANT_INSTREAM_FILE_STAR
	----------------------------
*/
class ANT_instream_file_star : public ANT_instream
{
private:
	long long bytes_read;
	FILE *file;

public:
	ANT_instream_file_star(FILE *file);
	virtual ~ANT_instream_file_star();

	virtual long long read(unsigned char *buffer, long long bytes);
} ;

#endif /* INSTREAM_FILE_H_ */
