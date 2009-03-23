/*
	DISK.H
	------
*/

#ifndef __DISK_H__
#define __DISK_H__

class ANT_disk_internals;

class ANT_disk
{
private:
	ANT_disk_internals *internals;
private:
	char *construct_full_path(char *filename);
	
public:
	ANT_disk();
	~ANT_disk();

	static char *read_entire_file(char *filename, long long *len = NULL);
	static char **buffer_to_list(char *buffer, long long *lines);
	char *get_first_filename(char *wildcard);
	char *get_next_filename(void);
} ;

#endif __DISK_H__
