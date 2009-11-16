/*
	DIRECTORY_ITERATOR_PKZIP_INTERNALS.H
	------------------------------------
*/
#ifndef DIRECTORY_ITERATOR_PKZIP_INTERNALS_H_
#define DIRECTORY_ITERATOR_PKZIP_INTERNALS_H_

#ifdef ANT_HAS_ZLIB
	#include "../zlib/zlib-1.2.3/zlib.h"
#endif

class ANT_directory_iterator_pkzip_internals
{
public:
#ifdef ANT_HAS_ZLIB
	z_stream stream;
#endif
} ;

/*
	class ANT_ZIP_END_OF_CENTRAL_DIRECTORY_RECORD
	---------------------------------------------
*/
class ANT_ZIP_end_of_central_directory_record
{
public:
	unsigned char signature[4];				// 0x06054b50 
	unsigned char disk_number[2];
	unsigned char disk_with_central_directory_start[2];
	unsigned char entries_in_central_directory_on_this_disk[2];
	unsigned char total_entries_in_central_directory[2];
	unsigned char central_directory_size[4];
	unsigned char offset_to_central_directory[4];
	unsigned char comment_length[2];
	/* then there is a character string comment */
} ;

/*
	class ANT_ZIP_LOCAL_FILE_HEADER
	-------------------------------
*/
class ANT_ZIP_local_file_header
{
public:
	unsigned char signature[4]; 				// 0x04034b50
	unsigned char version_needed_to_extract[2];
	unsigned char general_purpose_bit_flag[2];
	unsigned char compression_method[2];
	unsigned char last_mod_file_time[2];
	unsigned char last_mod_file_date[2];
	unsigned char crc32[4];
	unsigned char compressed_size[4];
	unsigned char uncompressed_size[4];
	unsigned char file_name_length[2];
	unsigned char extra_field_length[2];
} ;

/*
	class ANT_ZIP_ZIP64_END_OF_CENTRAL_DIRECTORY_LOCATOR
	----------------------------------------------------
*/
class ANT_ZIP_zip64_end_of_central_directory_locator
{
public:
	unsigned char signature[4];					// 0x07064b50
	unsigned char disk_with_end_central_directory_start[4];
	unsigned char offset_to_end_central_directory[8];
	unsigned char total_number_of_disks[4];
} ;

/*
	class ANT_ZIP_ZIP64_END_OF_CENTRAL_DIRECTORY_RECORD
	---------------------------------------------------
*/
class ANT_ZIP_zip64_end_of_central_directory_record
{
public:
	unsigned char signature[4];				// 0x06064b50
	unsigned char size_of_end_of_central_directory_record[8];
	unsigned char version_made_by[2];
	unsigned char version_needed_to_extract[2];
	unsigned char disk_number[4];
	unsigned char disk_with_central_directory_start[4];
	unsigned char entries_in_central_directory_on_this_disk[8];
	unsigned char total_entries_in_central_directory[8];
	unsigned char central_directory_size[8];
	unsigned char offset_to_central_directory[8];
	/* then there is the zip64 extensible data sector */
} ;


#endif /* DIRECTORY_ITERATOR_PKZIP_INTERNALS_H_ */
