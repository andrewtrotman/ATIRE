/*
	LINE.H
	------
*/
#ifndef LINE_H_
#define LINE_H_

/*
	class ANT_LINE
	--------------
*/
class ANT_line
{
public:
	enum { NONE = 0, END_OF_FILE };

public:
	long long attributes;
	char *text;

public:
	static ANT_line *buffer_to_list(char *contents, long long *lines_in_file);
} ;

#endif /* LINE_H_ */
