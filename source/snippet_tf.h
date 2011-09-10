/*
	SNIPPET_TF.H
	------------
*/
#ifndef SNIPPET_TF_H_
#define SNIPPET_TF_H_

#include "snippet.h"

class ANT_parser;

/*
	class ANT_SNIPPET_TF
	--------------------
*/
class ANT_snippet_tf : public ANT_snippet
{
private:
	ANT_parser *parser;							// the parser, used to classify tokens in the document
	unsigned long maximum_snippet_length;		// the length of the snippet (in bytes)
	long length_of_longest_document;			// the length of the longest document in the collection (in bytes)

public:
	ANT_snippet_tf(unsigned long max_length, long length_of_longest_document);
	virtual ~ANT_snippet_tf();

	virtual char *get_snippet(char *snippet, char *document, char *query);
} ;

#endif /* SNIPPET_TF_H_ */
