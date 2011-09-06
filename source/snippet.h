/*
	SNIPPET.H
	---------
*/
#ifndef SNIPPET_H_
#define SNIPPET_H_

/*
	class ANT_SNIPPET
	-----------------
*/
class ANT_snippet
{
public:
	ANT_snippet() {}
	virtual ~ANT_snippet() {}

	virtual char *get_snippet(char *snippet, char *document) = 0;
} ;


#endif /* SNIPPET_H_ */
