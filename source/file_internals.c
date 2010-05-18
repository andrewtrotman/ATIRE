/*
	FILE_INTERNALS.C
	----------------
*/
#include "file_internals.h"

/*
	ANT_FILE_INTERNALS::ANT_FILE_INTERNALS()
	----------------------------------------
*/
ANT_file_internals::ANT_file_internals()
{
#ifdef _MSC_VER
	fp = INVALID_HANDLE_VALUE;
#else
	fp = NULL;
#endif
}

/*
	ANT_FILE_INTERNALS::READ_FILE_64()
	----------------------------------
*/
#ifdef _MSC_VER
	int ANT_file_internals::read_file_64(HANDLE fp, void *destination, long long bytes_to_read)
	{
	unsigned char *from;
	const DWORD chunk_size = ((long long) 1 << (8 * sizeof(DWORD))) - 1;		// the compiler should convert this into a constant
	DWORD got_in_one_read;

	from = (unsigned char *)destination;
	while (bytes_to_read > chunk_size)
		{
		if (ReadFile(fp, from, chunk_size, &got_in_one_read, NULL) == 0)
			return 0;
		bytes_to_read -= chunk_size;
		from += chunk_size;
		}
	if (bytes_to_read > 0)			// catches a call to read 0 bytes 
		if (ReadFile(fp, from, (DWORD)bytes_to_read, &got_in_one_read, NULL) == 0)
			return 0;

	return 1;
	}
#else
	int ANT_file_internals::read_file_64(FILE *fp, void *destination, long long bytes_to_read)
	{
	return fread(destination, bytes_to_read, 1, fp);		// will return 0 (fail) of 1 (success)
	}

#endif

/*
	ANT_FILE_INTERNALS::WRITE_FILE_64()
	-----------------------------------
*/
#ifdef _MSC_VER
	int ANT_file_internals::write_file_64(HANDLE fp, void *destination, long long bytes_to_write)
	{
	unsigned char *from;
	const DWORD chunk_size = ((long long) 1 << (8 * sizeof(DWORD))) - 1;		// the compiler should convert this into a constant
	DWORD written;

	from = (unsigned char *)destination;
	while (bytes_to_write > chunk_size)
		{
		if (WriteFile(fp, from, chunk_size, &written, NULL) == 0)
			return 0;
		bytes_to_write -= chunk_size;
		from += chunk_size;
		}
	if (bytes_to_write > 0)			// catches a call to read 0 bytes 
		if (WriteFile(fp, from, (DWORD)bytes_to_write, &written, NULL) == 0)
			return 0;

	return 1;
	}
#else
	int ANT_file_internals::write_file_64(FILE *fp, void *destination, long long bytes_to_write)
	{
	return fwrite(destination, bytes_to_write, 1, fp);		// will return 0 (fail) of 1 (success)
	}

#endif
