/*
	DISK.H
	------
*/
#ifndef __DISK_H__
#define __DISK_H__

class ANT_disk_internals;
class ANT_disk_directory;

/*
	class ANT_DISK
	--------------
*/
class ANT_disk
{
protected:
	ANT_disk_internals *internals;

private:
	/*
		These routines are used for the get_next_filename routines
	*/
	char *construct_full_path(char *filename);

public:
	ANT_disk();
	virtual ~ANT_disk();

	static char *read_entire_file(char *filename, long long *len = NULL);
	static char **buffer_to_list(char *buffer, long long *lines);
	char *get_first_filename(char *wildcard);
	char *get_next_filename(void);

	virtual char *first(char *wildcard) { return get_first_filename(wildcard); }
	virtual char *next(void) { return get_next_filename(); }
} ;

#endif __DISK_H__
