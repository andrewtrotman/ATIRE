/*
	TAR.C
	-----
*/
#include <stdio.h>
#include <stdlib.h>
#include "tar.h"

/*
	class ANT_TAR_FILE_HEADER
	-------------------------
*/
class ANT_tar_file_header
{
public:
	/*
		UNIX verison 7 TAR format
	*/
	char filename[100];
	char file_mode[8];
	char owner_numeric_user_id[8];
	char group_numeric_user_id[8];
	char filesize_in_bytes[12];
	char last_modified_time_in_numeric_unix_time_format[12];
	char checksum_for_header_block[8];
	char file_type;
	char name_of_linked_file[100];
	/*
		Uniform Standard Tape Archive (UStar) format
	*/
	char ustar[6];
	char ustar_version[2];
	char owner_username[32];
	char owner_groupname[32];
	char device_major_number[8];
	char device_minor_number[8];
	char filename_prefix[155];
	char unused_reserved[12];
} ;

class ANT_stream_tar
{
private:
	static const long TAR_BLOCK_SIZE = 512;

private:
	FILE *fp;
	long long length_of_file_in_bytes;
	ANT_tar_file_header header;
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


/*
	ANT_STREAM_TAR::FILENAME()
	--------------------------
*/
char *ANT_stream_tar::filename(void)
{
if (header.filename_prefix[0] != '\0')
	{
	sprintf((char *)buffer, "%s/%s", header.filename_prefix, header.filename);
	return (char *)buffer;
	}
else
	return header.filename;
}

/*
	ANT_STREAM_TAR::DETOX()
	-----------------------
*/
inline long ANT_stream_tar::detox(char *octal)
{
long result;

result = *octal++ - '0';
result = result * 8 + (*octal++ - '0');
result = result * 8 + (*octal++ - '0');
result = result * 8 + (*octal++ - '0');
result = result * 8 + (*octal++ - '0');
result = result * 8 + (*octal++ - '0');
result = result * 8 + (*octal++ - '0');
result = result * 8 + (*octal++ - '0');
result = result * 8 + (*octal++ - '0');
result = result * 8 + (*octal++ - '0');
return result * 8 + (*octal++ - '0');
}

/*
	ANT_STREAM_TAR::FIRST()
	-----------------------
*/
long long ANT_stream_tar::first(char *filename)
{
bytes_read = 0;
if (file_open(filename))
	if (file_read((unsigned char *)&header, TAR_BLOCK_SIZE))
		return length_of_file_in_bytes = detox(header.filesize_in_bytes);
	else
		file_close();

return -1;
}

/*
	ANT_STREAM_TAR::NEXT()
	----------------------
*/
long long ANT_stream_tar::next(void)
{
long long extras;

/*
	If we're not at the end of the block then read one block at a time until we are.
*/
while (length_of_file_in_bytes - bytes_read > TAR_BLOCK_SIZE)
	file_read(buffer, TAR_BLOCK_SIZE);				// read to end of file

/*
	Now read up to the end of the  current block
*/
if ((extras = bytes_read % TAR_BLOCK_SIZE) != 0)
	file_read(buffer, TAR_BLOCK_SIZE - extras);

/*
	We've read no bytes from the current file
*/
bytes_read = 0;

/*
	Read the header of the next file
*/
if (file_read((unsigned char *)&header, TAR_BLOCK_SIZE))
	{
	if (header.filename[0] == '\0')
		{
		file_close();
		return -1;																// at end of file
		}
	else
		return length_of_file_in_bytes = detox(header.filesize_in_bytes);		// return the length
	}

/*
	Read has failed so we are probably at EOF (and the TAR file is broken)
*/
file_close();
return -2;
}

/*
	ANT_STREAM_TAR::FILE_OPEN()
	---------------------------
*/
long ANT_stream_tar::file_open(char *filename)
{
return (fp = fopen(filename, "rb")) != NULL;
}

/*
	ANT_STREAM_TAR::FILE_CLOSE()
	----------------------------
*/
long ANT_stream_tar::file_close(void)
{
fclose(fp);
return 1;
}

/*
	ANT_STREAM_TAR::FILE_READ()
	---------------------------
*/
unsigned char *ANT_stream_tar::file_read(unsigned char *buffer, long long size)
{
if (fread(buffer, (size_t)size, 1, fp))
	return buffer;
else
	return NULL;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_stream_tar tar_file;
long long size;

if (argc != 2)
	exit(printf("usage:%s <infile.tar>\n", argv[0]));

for (size = tar_file.first(argv[1]); size >= 0; size = tar_file.next())
	{
	puts(tar_file.filename());
	unsigned char *buffer = new unsigned char [(size_t)size];
	tar_file.read(buffer, size);
	delete buffer;
	}

return 0;
}
