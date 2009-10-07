/*
	DISK.H
	------
*/
#ifndef DISK_H_
#define DISK_H_

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

	static char *read_entire_file(char *filename, long long *len = 0);
	static char **buffer_to_list(char *buffer, long long *lines);
	char *get_first_filename(char *wildcard);
	char *get_next_filename(void);

	virtual char *first(char *wildcard) { return get_first_filename(wildcard); }
	virtual char *next(void) { return get_next_filename(); }
	virtual char *read_entire_file(long long *len);
} ;

#endif  /* DISK_H_ */
