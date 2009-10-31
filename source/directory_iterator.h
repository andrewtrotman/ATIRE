/*
	DIRECTORY_ITERATOR.H
	--------------------
*/
#ifndef DIRECTORY_ITERATOR_H_
#define DIRECTORY_ITERATOR_H_

class ANT_directory_iterator_internals;

/*
	class ANT_DIRECTORY_ITERATOR
	----------------------------
*/
class ANT_directory_iterator
{
private:
	char *current_filename;

protected:
	ANT_directory_iterator_internals *internals;

private:
	char *construct_full_path(char *filename);

public:
	ANT_directory_iterator();
	virtual ~ANT_directory_iterator();

	virtual char *first(char *wildcard);
	virtual char *next(void);
	virtual char *read_entire_file(long long *len = 0);
} ;

#endif  /* DIRECTORY_ITERATOR_H_ */

