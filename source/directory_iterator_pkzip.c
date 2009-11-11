/*
	DIRECTORY_ITERATOR_PKZIP.C
	--------------------------
*/
#include <stdio.h>
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
}

/*
	ANT_DIRECTORY_ITERATOR_PKZIP::~ANT_DIRECTORY_ITERATOR_PKZIP()
	-------------------------------------------------------------
*/
ANT_directory_iterator_pkzip::~ANT_directory_iterator_pkzip()
{
file->close();

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
#ifdef NEVER

unsigned char buffer[1024];
ANT_ZIP_local_file_header lfh;
uint16_t filename_length, extradata_length, method, flags, version;
uint32_t compressed_data_length, raw_data_length, signature;

if (files_read >= directory_files)
	return 0;

fread(&lfh, sizeof(lfh), 1, fp);
signature = ANT_get_unsigned_long_long(lfh.signature);
if (signature != 0x04034b50)
	puts("Broken Signature");

filename_length = ANT_get_unsigned_short(lfh.file_name_length);
fread(buffer, filename_length, 1, fp);
extradata_length = ANT_get_unsigned_short(lfh.extra_field_length);
compressed_data_length = ANT_get_unsigned_long(lfh.compressed_size);
raw_data_length = ANT_get_unsigned_long(lfh.uncompressed_size);
method = ANT_get_unsigned_short(lfh.compression_method);
flags = ANT_get_unsigned_short(lfh.general_purpose_bit_flag);
version = ANT_get_unsigned_short(lfh.version_needed_to_extract);

/*
	If size == 0 and verson == 10 then we have a directory.
*/
printf("%*.*s (%u bytes -> %u bytes (using:%d, v:%d))\n", filename_length, filename_length, buffer, compressed_data_length, raw_data_length, method, version);

read_and_toss(extradata_length);
read_and_toss(compressed_data_length);
if (flags &0x04)
	exit(printf("FOOTER"));

files_read++;
return 1;
#else
return object;
#endif
}

