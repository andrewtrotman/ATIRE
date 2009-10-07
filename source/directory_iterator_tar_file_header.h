/*
	DIRECTORY_ITERATOR_TAR_FILE_HEADER.H
	------------------------------------
*/

/*
	class DIRECTORY_ITERATOR_TAR_FILE_HEADER
	----------------------------------------
	This is the 512 byte file header seen at the beginning
	of each file in a TAR file.  The first half is from UNIX
	verion 7 and the second half is from POSIX UStar format
*/
class ANT_directory_iterator_tar_file_header
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
	char file_type;								// '0' or '\0' for file.
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
	/*
		Padding to make the size of this structure exactly 512 bytes in length
	*/
	char padding[12];
} ;
