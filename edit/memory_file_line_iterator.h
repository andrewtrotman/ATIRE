/*
	MEMORY_FILE_LINE_ITERATOR.H
	---------------------------
*/
#ifndef MEMORY_FILE_LINE_ITERATOR_H_
#define MEMORY_FILE_LINE_ITERATOR_H_

class ANT_line;
class ANT_memory_file_line;

/*
	class ANT_MEMORY_FILE_LINE_ITERATOR
	-----------------------------------
*/
class ANT_memory_file_line_iterator
{
private:
	ANT_memory_file_line *source;
	ANT_line *top_of_screen;

public:
	ANT_memory_file_line_iterator(ANT_memory_file_line *source);
	virtual ~ANT_memory_file_line_iterator();

	char *first(void);
	char *next(void);
};

#endif /* MEMORY_FILE_LINE_ITERATOR_H_ */
