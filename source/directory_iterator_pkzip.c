/*
	DIRECTORY_ITERATOR_PKZIP.C
	--------------------------
*/
#include <stdio.h>
#include <new>

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
directory = NULL;
zip_stream = false;
memory = new ANT_memory(1024 * 1024);
file = new ANT_file;

if (file->open(filename, "rb") == 0)
	exit(printf("Cannot open file:%s\n", filename));

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

delete [] directory;

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
	ANT_DIRECTORY_ITERATOR_PKZIP::READ_CENTRAL_DIRECTORY()
	------------------------------------------------------
*/
void ANT_directory_iterator_pkzip::read_central_directory(void)
{
ANT_directory_iterator_pkzip_internals::ANT_ZIP_central_directory_file_header file_header;
ANT_directory_iterator_pkzip_internals::ANT_ZIP_extra_field_header extras;
ANT_directory_iterator_pkzip_internals::ANT_ZIP_zip64_extended_information_extra_field *extra_field;
unsigned long long current;
uint32_t signature;
uint64_t compressed_size, uncompressed_size, offset;
uint16_t length, flags, id, size, got;

file->seek(directory_position);

if ((directory = new (std::nothrow) ANT_directory_iterator_pkzip_internals::ANT_ZIP_file_stats[(size_t)directory_length]) == NULL)
	exit(printf("ANT ZIP Reader Cannot allocate room for the central directory (%lld files)\n", directory_length));

for (current = 0; current < directory_files; current++)
	{
	file->read((unsigned char *)&file_header, sizeof(file_header));
	signature = ANT_get_unsigned_long(file_header.signature);
	if (signature != 0x02014b50)
		{
		exit(printf("ANT ZIP Reader found a broken signature (%lx) when expecting a file-header signature (%lx)\n", (unsigned long)signature, (unsigned long)0x02014b50));
		return;
		}
	flags = ANT_get_unsigned_short(file_header.general_purpose_bit_flag);

	/*
		Check the ZIP stream bit.  If its set then the contents of the directory must be read into memory
		because the archive doesn't have compressed and uncompressed sizes stored in the local_file_header.
		To get the compressed and uncompressed files its necessary to read them from the central directory.
	*/
	if (flags & 0x08)
		zip_stream = true;

	/*
		We're a zip-stream and so we need to keep track of the file details
	*/
	compressed_size = ANT_get_unsigned_long(file_header.compressed_size);
	uncompressed_size = ANT_get_unsigned_long(file_header.uncompressed_size);
	offset = ANT_get_unsigned_long(file_header.relative_offset_of_local_header);

	/*
		File name
	*/
	length = ANT_get_unsigned_short(file_header.file_name_length);
#ifdef NEVER
	file->read(buffer, length);
	printf("Found:%*.*s  ", length, length, buffer);
#else
	read_and_forget(length);
#endif

	/*
		Extra Field
	*/
	length = ANT_get_unsigned_short(file_header.extra_field_length);
	got = 0;
	while (got < length)
		{
		got += sizeof(extras);
		file->read((unsigned char *)&extras, sizeof(extras));

		id = ANT_get_unsigned_short(extras.id);
		size = ANT_get_unsigned_short(extras.size);
		got += size;
		if (size >= sizeof(buffer))
			read_and_forget(size);			// the extra field is too large so forget it
		else
			{
			file->read(buffer, size);
			if (id == 0x0001)			// ZIP64 extended information extra field
				{
				extra_field = (ANT_directory_iterator_pkzip_internals::ANT_ZIP_zip64_extended_information_extra_field *)buffer;
				if (compressed_size == 0xFFFFFFFF)
					compressed_size = ANT_get_unsigned_long_long(extra_field->compressed_size);
				if (uncompressed_size == 0xFFFFFFFF)
					uncompressed_size = ANT_get_unsigned_long_long(extra_field->uncompressed_size);
				if (offset == 0xFFFFFFFF)
					offset = ANT_get_unsigned_long_long(extra_field->relative_header_offset);
				}
			}
		}

	/*
		File comment
	*/
	//	printf("(%lld -> %lld)\n", compressed_size, uncompressed_size);
	length = ANT_get_unsigned_short(file_header.file_comment_length);
	read_and_forget(length);

	directory[current].location = offset;
	directory[current].compressed_size = compressed_size;
	directory[current].uncompressed_size = uncompressed_size;
	}
}

/*
	ANT_DIRECTORY_ITERATOR_PKZIP::FIRST()
	-------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_pkzip::first(ANT_directory_iterator_object *object)
{
files_read = 0;
read_central_directory_header();
read_central_directory();
file->seek(directory[0].location);			// seek to the start of the first file

return next(object);
}

/*
	ANT_DIRECTORY_ITERATOR_PKZIP::NEXT()
	------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_pkzip::next(ANT_directory_iterator_object *object)
{
ANT_directory_iterator_object *got;

do
	got = next_file(object);
while (got != NULL && got->length == 0);

return got;
}

/*
	ANT_DIRECTORY_ITERATOR_PKZIP::NEXT_FILE()
	-----------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_pkzip::next_file(ANT_directory_iterator_object *object)
{
unsigned long long disk_file_location;
ANT_directory_iterator_pkzip_internals::ANT_ZIP_local_file_header lfh;
ANT_directory_iterator_pkzip_internals::ANT_ZIP_data_descriptor data_descriptor;
unsigned char signature_bytes[4];
uint16_t filename_length, extradata_length, method, flags, version;
uint32_t compressed_data_length, raw_data_length, signature;
unsigned char *compressed_data;
#ifdef ANT_HAS_BZLIB
	unsigned int raw_data_length_for_bz2;
#endif

if (files_read >= directory_files)
	return NULL;

disk_file_location = file->tell();

file->read((unsigned char *)&lfh, sizeof(lfh));
signature = ANT_get_unsigned_long(lfh.signature);

if (signature == 0x02014b50)
	{
	printf("Warning:ANT ZIP Reader found the central directory before the number of files specified in the ZIP itself were found!\n");
	return NULL;		// we've found the central directory header (I'm not sure why we found it before we ran out of files though)
	}
if (signature != 0x04034b50)
	exit(printf("ANT ZIP Reader found a broken signature (%lx) when expecting a local-file-header signature (%lx)\n", (unsigned long)signature, (unsigned long)0x04034b50));
if ((filename_length = ANT_get_unsigned_short(lfh.file_name_length)) == 0)
	exit(printf("ANT ZIP Reader Cannot decompress files that don't have names\n"));
if (filename_length > PATH_MAX)
	exit(printf("ANT ZIP Reader Filename is %d characters long (which exceeds the Operating system limit of:%d\n", filename_length, PATH_MAX));
	
object->filename = new char[filename_length + 1];
file->read((unsigned char *)object->filename, filename_length);
object->filename[filename_length] = '\0';

if (zip_stream)
	{
	if (directory[files_read].location != disk_file_location)
		exit(printf("ANT ZIP Reader file in archive in unexpected location"));

	compressed_data_length = (uint32_t)directory[files_read].compressed_size;
	raw_data_length = (uint32_t)directory[files_read].uncompressed_size;
	}
else
	{
	compressed_data_length = ANT_get_unsigned_long(lfh.compressed_size);
	raw_data_length = ANT_get_unsigned_long(lfh.uncompressed_size);
	}

method = ANT_get_unsigned_short(lfh.compression_method);
flags = ANT_get_unsigned_short(lfh.general_purpose_bit_flag);
version = ANT_get_unsigned_short(lfh.version_needed_to_extract);

extradata_length = ANT_get_unsigned_short(lfh.extra_field_length);
read_and_forget(extradata_length);

files_read++;

//printf("%*.*s (%u bytes -> %u bytes (using:%d, v:%d))\n", filename_length, filename_length, object->filename, compressed_data_length, raw_data_length, method, version);

/*
	If the length of the source file is zero then skip it.
*/
if (raw_data_length > 0)
	{
	/*
		Now load and decompress the file
	*/
	if ((compressed_data = new (std::nothrow) unsigned char [compressed_data_length + 1]) == NULL)
		exit(printf("ANT ZIP Reader cannot allocate %lu bytes for the compressed file\n", (unsigned long)compressed_data_length));

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
	delete [] compressed_data;
	}

object->length = raw_data_length;

/*
	Finally, we expect a ZIP Data Descriptor if we're in a zip stream
*/
if (zip_stream && raw_data_length > 0)
	{
	file->read(signature_bytes, sizeof(signature_bytes));
	signature = ANT_get_unsigned_long(signature_bytes);
	if (signature == 0x08074b50)
		file->read((unsigned char *)&data_descriptor, sizeof(data_descriptor));
	else
		read_and_forget(sizeof(data_descriptor) - sizeof(signature_bytes));
	}

return object;
}
