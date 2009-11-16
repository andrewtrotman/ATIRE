/*
	DIRECTORY_ITERATOR_PKZIP.C
	--------------------------
*/
#include <stdio.h>

#ifdef ANT_HAS_ZLIB
	#include "../zlib/zlib-1.2.3/zlib.h"
#endif

#include "directory_iterator_pkzip_internals.h"
#include "directory_iterator_pkzip.h"
#include "directory_iterator_object.h"
#include "memory.h"
#include "file.h"

/*
	ANT_DIRECTORY_ITERATOR_PKZIP::ANT_DIRECTORY_ITERATOR_PKZIP()
	------------------------------------------------------------
*/
ANT_directory_iterator_pkzip::ANT_directory_iterator_pkzip(char *filename)
{
memory = new ANT_memory(1024 * 1024);
file = new ANT_file(memory);
file->open(filename, "rb");
read_central_directory_header();
internals = new ANT_directory_iterator_pkzip_internals;

#ifdef ANT_HAS_ZLIB
	internals->stream.zalloc = Z_NULL;
	internals->stream.zfree = Z_NULL;
	internals->stream.opaque = Z_NULL;
	inflateInit2(&internals->stream, -MAX_WBITS);
#endif
}

/*
	ANT_DIRECTORY_ITERATOR_PKZIP::~ANT_DIRECTORY_ITERATOR_PKZIP()
	-------------------------------------------------------------
*/
ANT_directory_iterator_pkzip::~ANT_directory_iterator_pkzip()
{
file->close();

#ifdef ANT_HAS_ZLIB
	inflateEnd(&internals->stream);
#endif

delete internals;

delete file;
delete memory;
}

/*
	ANT_DIRECTORY_ITERATOR_PKZIP::READ_CENTRAL_DIRECTORY_HEADER()
	-------------------------------------------------------------
*/
unsigned long long ANT_directory_iterator_pkzip::read_central_directory_header(void)
{
long long start_of_end;
uint32_t signature;
ANT_ZIP_end_of_central_directory_record eocdr;
ANT_ZIP_zip64_end_of_central_directory_record z64_eocdr;
ANT_ZIP_zip64_end_of_central_directory_locator z64_eocd_locate;

start_of_end = file->file_length();
file->seek(start_of_end - sizeof(eocdr));
file->read((unsigned char *)&eocdr, sizeof(eocdr));
signature = ANT_get_unsigned_long(eocdr.signature);
//printf("SIG %x\n", signature);
if (signature != 0x06054b50)
	{
	/*
		This is the case that there is a comment at the end of the PKZIP file
		and we've not written code to account for that instance.  It's basicly
		a linear search backwards in this case.
	*/
	puts("ANT ZIP Reader does not support zip files that have comments at the end - sorry");
	directory_position = directory_length = directory_files = 0;
	}
else
	{
	directory_position = ANT_get_unsigned_long(eocdr.offset_to_central_directory);
	directory_length = ANT_get_unsigned_long(eocdr.central_directory_size);
	directory_files = ANT_get_unsigned_short(eocdr.total_entries_in_central_directory);
	}

if (directory_files == 0xFFFF)
	{
	/*
		We're a ZIP64 file and there are more than 0xFFFF files in the archive
	*/
	file->seek(start_of_end - sizeof(eocdr) - sizeof(z64_eocd_locate));
	file->read((unsigned char *)&z64_eocd_locate, sizeof(z64_eocd_locate));
	if (ANT_get_unsigned_long(z64_eocd_locate.signature) == 0x07064b50)
		{
		/*
			In this case we are a ZIP64 file - the other case is that there really are 0xFFFF files in the ZIP archive.
		*/
		if (ANT_get_unsigned_long(z64_eocd_locate.total_number_of_disks) != 1)
			{
			puts("ANT ZIP Reader does not support zip files that span multiple disks - sorry");
			directory_files = 0;
			}
		else
			{
			file->seek(ANT_get_unsigned_long_long(z64_eocd_locate.offset_to_end_central_directory));
			file->read((unsigned char *)&z64_eocdr, sizeof(z64_eocdr));
			
			directory_position = ANT_get_unsigned_long_long(eocdr.offset_to_central_directory);
			directory_length = ANT_get_unsigned_long_long(eocdr.central_directory_size);
			directory_files = ANT_get_unsigned_long_long(eocdr.total_entries_in_central_directory);
			}
		}
	}

return directory_files;
}

/*
	ANT_DIRECTORY_ITERATOR_PKZIP::READ_AND_FORGET()
	-----------------------------------------------
*/
void ANT_directory_iterator_pkzip::read_and_forget(unsigned long long bytes)
{
unsigned long long goes;

for (goes = 0; goes < bytes / sizeof(buffer); goes++)
	file->read(buffer, sizeof(buffer));

file->read(buffer, bytes % sizeof(buffer));
}

/*
	ANT_DIRECTORY_ITERATOR_PKZIP::FIRST()
	-------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_pkzip::first(ANT_directory_iterator_object *object, char *wildcard, long get_file)
{
files_read = 0;
read_central_directory_header();
file->seek(0);

return next(object, get_file);
}

/*
	ANT_DIRECTORY_ITERATOR_PKZIP::NEXT()
	------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_pkzip::next(ANT_directory_iterator_object *object, long get_file)
{
ANT_ZIP_local_file_header lfh;
uint16_t filename_length, extradata_length, method, flags, version;
uint32_t compressed_data_length, raw_data_length, signature;
unsigned char *compressed_data;

if (files_read >= directory_files)
	return NULL;

file->read((unsigned char *)&lfh, sizeof(lfh));
signature = ANT_get_unsigned_long(lfh.signature);
if (signature != 0x04034b50)
	{
	printf("ANT ZIP Reader found a broken signature (%lx) when expecting a local-file-header signature (%lx)\n", signature, (long)0x04034b50);
	return NULL;
	}

if ((filename_length = ANT_get_unsigned_short(lfh.file_name_length)) == 0)
	{
	printf("ANT ZIP Reader Cannot decompress files that don't have names\n");
	return NULL;
	}
if (filename_length > PATH_MAX)
	{
	printf("ANT ZIP Reader Filename is %d characters long (which exceeds the Operating system limit of:%d\n", filename_length, PATH_MAX);
	return NULL;
	}
	
file->read((unsigned char *)object->filename, filename_length);
object->filename[filename_length] = '\0';

compressed_data_length = ANT_get_unsigned_long(lfh.compressed_size);
raw_data_length = ANT_get_unsigned_long(lfh.uncompressed_size);
method = ANT_get_unsigned_short(lfh.compression_method);
flags = ANT_get_unsigned_short(lfh.general_purpose_bit_flag);
version = ANT_get_unsigned_short(lfh.version_needed_to_extract);

extradata_length = ANT_get_unsigned_short(lfh.extra_field_length);
read_and_forget(extradata_length);

files_read++;

/*
	If the length of the source file is zero then skip it.
*/
if (raw_data_length == 0)
	return next(object, get_file);

printf("%*.*s (%u bytes -> %u bytes (using:%d, v:%d))\n", filename_length, filename_length, object->filename, compressed_data_length, raw_data_length, method, version);

/*
	Now load and decompress the file
*/
if ((compressed_data = new (std::nothrow) unsigned char [compressed_data_length + 1]) == NULL)
	{
	printf("ANT ZIP Reader cannot allocate %ld bytes for the compressed file\n", compressed_data_length);
	return NULL;
	}
file->read(compressed_data, compressed_data_length);

if (method == PKZIP_METHOD_STORED)
	{
	compressed_data[compressed_data_length] = '\0';
	if (get_file)
		object->file = (char *)compressed_data;
	else
		delete [] compressed_data;
	}
else if (method == PKZIP_METHOD_DEFLATE)
	{
#ifdef ANT_HAS_ZLIB
	/*
		Call the Zlib to decompress
	*/
	object->file = new char [raw_data_length];

	internals->stream.avail_in = compressed_data_length;
	internals->stream.next_in = (Bytef *)compressed_data;
	internals->stream.avail_out = raw_data_length;
	internals->stream.next_out = (Bytef *)object->file;

	inflateReset(&internals->stream);
	inflate(&internals->stream, Z_SYNC_FLUSH);

	delete [] compressed_data;
#else
	exit(printf("You are trying to decompress a zip file but ZLIB is not included in this build"));
/*
	object->filename = '\0';
	object->file = NULL;
	delete [] compressed_data;
	return NULL;
*/
#endif
	}
else if (method == PKZIP_METHOD_BZ2)
	{
	}

if (flags &0x04)
	exit(printf("FOOTER"));

return object;
}
