/*
	DIRECTORY_ITERATOR_PKZIP.C
	--------------------------
*/
#include <stdio.h>

#ifdef ANT_HAS_ZLIB
	#include "zlib.h"
#endif
#ifdef ANT_HAS_BZLIB
	#include "bzlib.h"
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
ANT_directory_iterator_pkzip::ANT_directory_iterator_pkzip(char *filename, long get_file) : ANT_directory_iterator(filename, get_file)
{
memory = new ANT_memory(1024 * 1024);
file = new ANT_file;
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
uint32_t signature = 0;
ANT_directory_iterator_pkzip_internals::ANT_ZIP_end_of_central_directory_record eocdr;
ANT_directory_iterator_pkzip_internals::ANT_ZIP_zip64_end_of_central_directory_record z64_eocdr;
ANT_directory_iterator_pkzip_internals::ANT_ZIP_zip64_end_of_central_directory_locator z64_eocd_locate;

start_of_end = file->file_length();
if (start_of_end == 0)
	exit(printf("ANT ZIP Reader cannot read file (file size if reported as 0)\n"));

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
			
			directory_position = ANT_get_unsigned_long_long(z64_eocdr.offset_to_central_directory);
			directory_length = ANT_get_unsigned_long_long(z64_eocdr.central_directory_size);
			directory_files = ANT_get_unsigned_long_long(z64_eocdr.total_entries_in_central_directory);
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
ANT_directory_iterator_object *ANT_directory_iterator_pkzip::first(ANT_directory_iterator_object *object)
{
files_read = 0;
read_central_directory_header();
file->seek(0);

return next(object);
}

/*
	ANT_DIRECTORY_ITERATOR_PKZIP::NEXT()
	------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_pkzip::next(ANT_directory_iterator_object *object)
{
ANT_directory_iterator_pkzip_internals::ANT_ZIP_local_file_header lfh;
uint16_t filename_length, extradata_length, method, flags, version;
uint32_t compressed_data_length, raw_data_length, signature;
unsigned char *compressed_data;
#ifdef ANT_HAS_BZLIB
	unsigned int raw_data_length_for_bz2;
#endif

if (files_read >= directory_files)
	return NULL;

file->read((unsigned char *)&lfh, sizeof(lfh));
signature = ANT_get_unsigned_long(lfh.signature);

if (signature == 0x02014b50)
	{
	printf("Warning:ANT ZIP Reader found the central directory before the number of files specified in the ZIP itself were found!\n");
	return NULL;		// we've found the central directory header (I'm not sure why we found it before we ran out of files though)
	}
if (signature != 0x04034b50)
	exit(printf("ANT ZIP Reader found a broken signature (%lx) when expecting a local-file-header signature (%lx)\n", signature, (long)0x04034b50));
if ((filename_length = ANT_get_unsigned_short(lfh.file_name_length)) == 0)
	exit(printf("ANT ZIP Reader Cannot decompress files that don't have names\n"));
if (filename_length > PATH_MAX)
	exit(printf("ANT ZIP Reader Filename is %d characters long (which exceeds the Operating system limit of:%d\n", filename_length, PATH_MAX));
	
file->read((unsigned char *)object->filename, filename_length);
object->filename[filename_length] = '\0';

compressed_data_length = ANT_get_unsigned_long(lfh.compressed_size);
raw_data_length = ANT_get_unsigned_long(lfh.uncompressed_size);
method = ANT_get_unsigned_short(lfh.compression_method);
flags = ANT_get_unsigned_short(lfh.general_purpose_bit_flag);
version = ANT_get_unsigned_short(lfh.version_needed_to_extract);

if (flags & 0x04)
	exit(printf("ANT ZIP Reader cannot decompress ZIPped streams\n"));

extradata_length = ANT_get_unsigned_short(lfh.extra_field_length);
read_and_forget(extradata_length);

files_read++;

/*
	If the length of the source file is zero then skip it.
*/
if (raw_data_length == 0)
	return next(object);

//printf("%*.*s (%u bytes -> %u bytes (using:%d, v:%d))\n", filename_length, filename_length, object->filename, compressed_data_length, raw_data_length, method, version);

/*
	Now load and decompress the file
*/
if ((compressed_data = new (std::nothrow) unsigned char [compressed_data_length + 1]) == NULL)
	exit(printf("ANT ZIP Reader cannot allocate %lu bytes for the compressed file\n", compressed_data_length));

file->read(compressed_data, compressed_data_length);

if (method == PKZIP_METHOD_STORED)
	{
	if (get_file)
		{
		object->file = (char *)compressed_data;
		compressed_data = NULL;
		}
	}
else if (method == PKZIP_METHOD_DEFLATE)
	{
#ifdef ANT_HAS_ZLIB
	object->file = new char [raw_data_length + 1];

	internals->stream.avail_in = compressed_data_length;
	internals->stream.next_in = (Bytef *)compressed_data;
	internals->stream.avail_out = raw_data_length;
	internals->stream.next_out = (Bytef *)object->file;

	if (inflateReset(&internals->stream) != Z_OK)
		exit(printf("Cannot reset the DEFLATE decompressor\n"));
	if (inflate(&internals->stream, Z_SYNC_FLUSH) != Z_STREAM_END)
		exit(printf("Cannot INFLATE document:%s\n", object->filename));
#else
	exit(printf("You are trying to decompress a zip file but ZLIB is not included in this build"));
#endif
	}
else if (method == PKZIP_METHOD_BZ2)
	{
#ifdef ANT_HAS_BZLIB
	object->file = new char [raw_data_length + 1];
	raw_data_length_for_bz2 = raw_data_length;
	if (BZ2_bzBuffToBuffDecompress(object->file, &raw_data_length_for_bz2, (char *)compressed_data, compressed_data_length, 0, 0) != BZ_OK)
		exit(printf("Cannot BZ2 Decompress document:%s\n", object->filename));
#else
	exit(printf("You are trying to decompress a zip file but BZLIB is not included in this build"));
#endif
	}
else
	exit(printf("You are trying to decompress a zip file but ANT does not have a suitable decompressor\n"));

object->file[raw_data_length] = '\0';
object->length = raw_data_length;
delete [] compressed_data;

return object;
}
