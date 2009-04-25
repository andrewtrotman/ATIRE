/*
	DISK.H
	------
*/

#ifndef __DISK_H__
#define __DISK_H__

class ANT_disk_internals;
class ANT_disk_directory;

class ANT_disk
{
private:
	ANT_disk_internals *internals;
	ANT_disk_directory *handle_stack;
	ANT_disk_directory *recursive_file_list;
	char *wildcard;

private:
	/*
		These routines are used for the get_next_filename routines
	*/
	char *construct_full_path(char *filename);

	/*
		These routines are used for the recursive_get_next_filename routines
	*/
	long push_directory(void);
	long pop_directory(void);
	char *recursive_get_next_match_wildcard(long at_end);
	char *recursive_get_first_filename(char *root_directory, char *local_directory);

public:
	ANT_disk();
	~ANT_disk();

	static char *read_entire_file(char *filename, long long *len = NULL);
	static char **buffer_to_list(char *buffer, long long *lines);
	char *get_first_filename(char *wildcard);
	char *get_next_filename(void);

	char *recursive_get_first_filename(char *wildcard);
	char *recursive_get_next_filename(void);
} ;

#endif __DISK_H__
