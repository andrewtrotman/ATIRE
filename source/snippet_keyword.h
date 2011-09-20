/*
	SNIPPET_KEYWORD.H
	-----------------
*/
#ifndef SNIPPET_KEYWORD_H_
#define SNIPPET_KEYWORD_H_

/*
	class ANT_SNIPPET_KEYWORD
	-------------------------
*/
class ANT_snippet_keyword
{
public:
	ANT_string_pair keyword;			// the key word
	double score;						// the score of the keyword (for example its IDF score)
} ;

#endif /* SNIPPET_KEYWORD_H_ */
