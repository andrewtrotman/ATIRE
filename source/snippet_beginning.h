/*
	SNIPPET_BEGINNING.H
	-------------------
*/
#ifndef SNIPPET_BEGINNING_H_
#define SNIPPET_BEGINNING_H_

class ANT_parser;
/*
	class ANT_SNIPPET_BEGINNING
	---------------------------
*/
class ANT_snippet_beginning
{
private:
	ANT_parser *parser;
	unsigned long maximum_snippet_length;		// in bytes

public:
	ANT_snippet_beginning(unsigned long max_length);
	virtual ~ANT_snippet_beginning();

	virtual char *get_snippet(char *snippet, char *document);
} ;



#endif /* SNIPPET_BEGINNING_H_ */
