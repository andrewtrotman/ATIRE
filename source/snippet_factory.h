/*
	SNIPPET_FACTORY.H
	-----------------
*/
#ifndef SNIPPET_FACTORY_H_
#define SNIPPET_FACTORY_H_

class ANT_snippet;

/*
	class ANT_SNIPPET_FACTORY
	-------------------------
*/
class ANT_snippet_factory
{
public:
	enum { SNIPPET_TITLE = 1, SNIPPET_BEGINNING };

public:
	ANT_snippet_factory() {}
	virtual ~ANT_snippet_factory() {}

	static ANT_snippet *get_snippet_maker(long type, long length, char *tag);
} ;

#endif /* SNIPPET_FACTORY_H_ */
