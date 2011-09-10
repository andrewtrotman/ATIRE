/*
	SNIPPET_BEGINNING.H
	-------------------
*/
#ifndef SNIPPET_BEGINNING_H_
#define SNIPPET_BEGINNING_H_


#include "snippet.h"

class ANT_parser;

/*
	class ANT_SNIPPET_BEGINNING
	---------------------------
*/
class ANT_snippet_beginning : public ANT_snippet
{
private:
	ANT_parser *parser;							// the parser, used to classify tokens in the document
	unsigned long maximum_snippet_length;		// in bytes
	char *tag;									// ignore all text up-to the end of this tag
	long tag_length;							// this is the length of this->tag

public:
	ANT_snippet_beginning(unsigned long max_length, char *tag);
	virtual ~ANT_snippet_beginning();

	virtual char *get_snippet(char *snippet, char *document, char *query);
} ;

#endif /* SNIPPET_BEGINNING_H_ */
