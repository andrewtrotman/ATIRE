/*
	INSTREAM_FILE_STAR.C
	--------------------
*/
#include "instream_file_star.h"

/*
	ANT_INSTREAM_FILE_STAR::ANT_INSTREAM_FILE_STAR()
	------------------------------------------------
*/
ANT_instream_file_star::ANT_instream_file_star(FILE *infile) : ANT_instream(NULL)
{
file = infile;
bytes_read = 0;
}

/*
	ANT_INSTREAM_FILE_STAR::~ANT_INSTREAM_FILE_STAR()
	-------------------------------------------------
*/
ANT_instream_file_star::~ANT_instream_file_star()
{
}

/*
	ANT_INSTREAM_FILE_STAR::READ()
	------------------------------
*/
long long ANT_instream_file_star::read(unsigned char *buffer, long long bytes)
{
bytes_read += bytes;
return fread(buffer, (size_t)bytes, 1, file);
}
