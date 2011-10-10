/*
	MEMORY_FILE_LINE.H
	------------------
*/
#ifndef MEMORY_FILE_LINE_H_
#define MEMORY_FILE_LINE_H_

/*
	class ANT_MEMORY_FILE_LINE
	--------------------------
*/
class ANT_memory_file_line
{
private:
	unsigned char *contents;
	unsigned char **line;

public:
	ANT_memory_file_line() {}
	virtual ~ANT_memory_file_line() {}

	/*
		Read a file into memory and return the number of lines read
	*/
	long long read_file(char *filename);
} ;

#endif /* MEMORY_FILE_LINE_H_ */
