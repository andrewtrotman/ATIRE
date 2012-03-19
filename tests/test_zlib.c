/*
 * 14-March-2012
 *
 * http://zlib.net/zlib_how.html
 * http://zlib.net/zpipe.c
 *
 * http://zlib.net/manual.html
 *
 * http://www.lemoda.net/c/zlib-open-write/index.html
 *
 * http://stackoverflow.com/questions/1838699/how-can-i-decompress-a-gzip-stream-with-zlib
 *
 *
 * gcc -o test_zlib test_zlib.c -lz -g -ggdb && ./test_zlib --compress --in-file test_zlib.c --out-file output.zlib
 *
 * gcc -o test_zlib test_zlib.c -lz -g -ggdb && ./test_zlib --compress-to-gzip --in-file hello.txt --out-file hello.gz
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "zlib.h"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
	#include <fcntl.h>
	#include <io.h>
	#define SET_BINATY_MODE(file) setmode(fileno(file), O_BINARY)
#else
	#define SET_BINATY_MODE(file)
#endif

#define CHUNK 16384

#define windowBits 15
#define GZIP_ENCODING 16
#define memLevel 8

typedef enum {NONE, COMPRESS, COMPRESS_TO_GZIP, DECOMPRESS, DECOMPRESS_FROM_GZIP} operation_t;

/*
 * compress from source to destination
 */
int opt_compress(FILE *source, FILE *destination, int level, operation_t the_operation) {
	int ret, flush;
	unsigned compressed_size;
	z_stream stream;
	unsigned char in_buffer[CHUNK];
	unsigned char out_buffer[CHUNK];

	// allocate default state
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	if (the_operation == COMPRESS) {
		ret = deflateInit(&stream, level);
	} else if (the_operation == COMPRESS_TO_GZIP) {
		ret = deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, windowBits | GZIP_ENCODING, memLevel, Z_DEFAULT_STRATEGY);
	}
	if (ret != Z_OK) {
		return ret;
	}

	//
	//compress until enf of the file
	//
	do {
		stream.next_in = in_buffer;
		stream.avail_in = fread(in_buffer, 1, CHUNK, source);
		if (ferror(source)) {
			deflateEnd(&stream);
			return Z_ERRNO;
		}

		flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;

		// use a do-while loop in case of the out buffer has a smaller
		// size to compress the in buffer in one go.
		do {
			stream.avail_out = CHUNK;
			stream.next_out = out_buffer;
			ret = deflate(&stream, flush);
			assert(ret != Z_STREAM_ERROR);
			compressed_size = CHUNK - stream.avail_out;
			if (fwrite(out_buffer, 1, compressed_size, destination) != compressed_size || ferror(destination)) {
				deflateEnd(&stream);
				return Z_ERRNO;
			}
		} while (stream.avail_out == 0);

		assert(stream.avail_in == 0);
	} while (flush != Z_FINISH);

	assert(ret == Z_STREAM_END);

	deflateEnd(&stream);
	return Z_OK;
}


/*
 * decompress from file source to destination
 */
int opt_decompress(FILE *source, FILE *destination, operation_t the_operation) {
 	int ret;
 	unsigned decompressed_size;
 	z_stream stream;
 	unsigned char in_buffer[CHUNK];
 	unsigned char out_buffer[CHUNK];

 	// allocate inflate state
 	stream.zalloc = Z_NULL;
 	stream.zfree = Z_NULL;
 	stream.opaque = Z_NULL;
 	stream.avail_in = 0;
 	stream.next_in = Z_NULL;
	if (the_operation == DECOMPRESS) {
		ret = inflateInit(&stream);
	} else if (the_operation == DECOMPRESS_FROM_GZIP) {
		ret = inflateInit2(&stream, windowBits + GZIP_ENCODING);
	}
 	if (ret != Z_OK) {
 		return ret;
 	}

	// decompress until deflate stream ends or end of file
	do {
		stream.next_in = in_buffer;
		stream.avail_in = fread(in_buffer, 1, CHUNK, source);
		if (ferror(source)) {
			inflateEnd(&stream);
			return Z_ERRNO;
		}
		if (stream.avail_in == 0) {
			break;
		}

		// use a do-while loop in case of the out buffer has a smaller
		// size to hold the decompress data in the in buffer in one go.
		do {
			stream.avail_out = CHUNK;
			stream.next_out = out_buffer;
			ret = inflate(&stream, Z_NO_FLUSH);
			assert(ret != Z_STREAM_ERROR);

			switch(ret) {
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR; // and fall through
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					inflateEnd(&stream);
					return ret;
			}
			decompressed_size = CHUNK - stream.avail_out;

			if (fwrite(out_buffer, 1, decompressed_size, destination) != decompressed_size || ferror(destination)) {
				inflateEnd(&stream);
				return Z_ERRNO;
			}
		} while (stream.avail_out == 0);
	} while (ret != Z_STREAM_END);

 	// clean up and return
 	inflateEnd(&stream);
 	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

void z_error_check(int ret, FILE *source, FILE *destination) {
	switch(ret) {
		case Z_ERRNO:
			if (ferror(source)) {
				fprintf(stderr, "ERROR: reading source file\n");
			}
			if (ferror(destination)) {
				fprintf(stderr, "ERROR: reading destination file\n");
			}
			break;
		case Z_STREAM_ERROR:
			fprintf(stderr, "ERROR: invalid compression level\n");
			break;
		case Z_DATA_ERROR:
			fprintf(stderr, "ERROR: invalid or incomplete deflate data\n");
			break;
		case Z_MEM_ERROR:
			fprintf(stderr, "ERROR: out of memory\n");
			break;
		case Z_VERSION_ERROR:
			fprintf(stderr, "ERROR: zlib version mismatch!\n");
	}
}

int main(int argc, char *argv[]) {
	int i, ret;
	operation_t operation = NONE;
	char *in_file_name = NULL, *out_file_name = NULL;
	FILE *in_file = NULL, *out_file = NULL;

	if (argc <= 1) {
		printf("please specify arguments\n");
		exit(2);
	}

	i = 1;
	while (i < argc) {
		if(strcmp(argv[i], "--compress") == 0) {
			operation = COMPRESS;
			i++;
		} else if (strcmp(argv[i], "--decompress") == 0) {
			operation = DECOMPRESS;
			i++;
		} else if (strcmp(argv[i], "--compress-to-gzip") == 0) {
			operation = COMPRESS_TO_GZIP;
			i++;
		} else if (strcmp(argv[i], "--decompress-from-gzip") == 0) {
			operation = DECOMPRESS_FROM_GZIP;
			i++;
		} else if (strcmp(argv[i], "--in-file") == 0) {
			i++;
			in_file_name = argv[i];
			i++;
		} else if (strcmp(argv[i], "--out-file") == 0) {
			i++;
			out_file_name = argv[i];
			i++;
		} else {
			fprintf(stderr, "ERROR: the arguemnt \"%s\" is not supported\n", argv[i]);
			exit(2);
		}
	}

	if ((operation == NONE) || (in_file_name == NULL) || (out_file_name == NULL)) {
		printf("please specify arguments\n");
		exit(2);
	}

	in_file = fopen(in_file_name, "rb");
	out_file = fopen(out_file_name, "wb");
	printf("in_file_name: %s, out_file_name: %s\n", in_file_name, out_file_name);

	SET_BINATY_MODE(in_file);
	SET_BINATY_MODE(out_file_name);


	switch (operation) {
		case COMPRESS:
		case COMPRESS_TO_GZIP:
			ret = opt_compress(in_file, out_file, Z_DEFAULT_COMPRESSION, operation);
			z_error_check(ret, in_file, out_file);
			break;
		case DECOMPRESS:
		case DECOMPRESS_FROM_GZIP:
			ret = opt_decompress(in_file, out_file, operation);
			if (ret != Z_OK) {
				z_error_check(ret, in_file, out_file);
			}
			break;
		default:
			fprintf(stderr, "ERROR: the operation is not supported\n");
			break;
	}

	fclose(in_file);
	fclose(out_file);
	return 0;
}
