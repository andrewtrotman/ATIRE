/*
	DIRECTORY_ITERATOR_DEFLATE.C
	----------------------------
*/
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include "directory_iterator_deflate.h"
#include "directory_iterator_scrub.h"
#include "compress_text_deflate.h"

#ifdef ANT_HAS_ZLIB
	#include "zlib.h"
#endif

/*
	ANT_DIRECTORY_ITERATOR_DEFLATE::ANT_DIRECTORY_ITERATOR_DEFLATE()
	----------------------------------------------------------------
*/
ANT_directory_iterator_deflate::ANT_directory_iterator_deflate(ANT_directory_iterator *source, long mode) : ANT_directory_iterator()
{
this->mode = mode;
this->source = source;
decompressor = new ANT_compress_text_deflate();
largest_decompressed_file = INITIAL_LARGEST_DECOMPRESSED_FILE_SIZE;
#ifndef ANT_HAS_ZLIB
	exit(printf("You are trying to decompress a zip file but ZLIB is not included in this build"));
#endif
}

/*
	ANT_DIRECTORY_ITERATOR_DEFLATE::~ANT_DIRECTORY_ITERATOR_DEFLATE()
	-----------------------------------------------------------------
*/
ANT_directory_iterator_deflate::~ANT_directory_iterator_deflate()
{
delete decompressor;
delete source;
}

/*
	ANT_DIRECTORY_ITERATOR_DEFLATE::PROCESS()
	-----------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_deflate::process(ANT_directory_iterator_object *object)
{
#ifdef ANT_HAS_ZLIB
	char *decompressed, *end_of_decompressed;
	size_t decompressed_length;
	int status;
	z_stream stream;

	if (object == NULL)
		return NULL;

	do
		{
		decompressed_length = (size_t)largest_decompressed_file;
		// 1 byte for the null terminator
		if ((decompressed = new (std::nothrow) char [decompressed_length + 1]) == NULL)
			{
			printf("Failed to decompress file:%s ... skiping (tried:%lld)\n", object->filename, (long long)largest_decompressed_file);
			largest_decompressed_file = INITIAL_LARGEST_DECOMPRESSED_FILE_SIZE;

			/*
				Get the next file and continue
			*/
			if ((object = next(object)) == NULL)
				return NULL;
			}

		stream.avail_in = (uInt)object->length;
		stream.next_in = (Bytef *)object->file;
		stream.avail_out = (uInt)decompressed_length;
		stream.next_out = (Bytef *)decompressed;
		stream.zalloc = Z_NULL;
		stream.zfree = Z_NULL;
		stream.opaque = Z_NULL;
		inflateInit2(&stream, 15 + 32);

		status = inflate(&stream, Z_NO_FLUSH);
		if (status != Z_STREAM_END)
			{
			largest_decompressed_file *= 2;
			delete [] decompressed;
			decompressed = NULL;
			}
		end_of_decompressed = (char *)stream.next_out;
		inflateEnd(&stream);
		}
	while (decompressed == NULL);

	delete [] object->file;
	delete [] object->filename;

	object->file = decompressed;
	decompressed_length = end_of_decompressed - decompressed;
	object->file[decompressed_length] = '\0';			// '\0' terminate the input
	object->length = decompressed_length;

	return object;
#else
	return object;
#endif
}
