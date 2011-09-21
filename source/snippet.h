/*
	SNIPPET.H
	---------
*/
#ifndef SNIPPET_H_
#define SNIPPET_H_

#include "NEXI_ant.h"
#include "snippet_keyword.h"
#include "btree_iterator.h" 		// for MAX_TERM_LENGTH

class ANT_NEXI_term_ant;
class ANT_parser;
class ANT_stem;

/*
	class ANT_SNIPPET
	-----------------
*/
class ANT_snippet
{
protected:
	long length_of_longest_document;			// length of the longest document in the colletion (in bytes)
	unsigned long maximum_snippet_length;		// length of the snippet (in bytes)
	ANT_parser *parser;							// parser used to parse documents
	ANT_NEXI_ant NEXI_parser;					// parser used to parse queries
	ANT_snippet_keyword *keyword_hit;			// list long enough to hold a pointer to each keyword in the document
	char unstemmed_term[MAX_TERM_LENGTH];		// if we're going to stem them we need temporary storage so that we can call the stemmer... this is it.
	char stemmed_term[MAX_TERM_LENGTH];			// the stemmed term (output from the stemmer)

protected:
	ANT_NEXI_term_ant **generate_term_list(char *query, long *terms_in_query, ANT_stem *stemmer);
	char *next_n_characters_after(char *snippet, long maximum_snippet_length, char *starting_point = NULL);
	static int cmp_term(const void *a, const void *b);		// a is a (ANT_parser_token *) and b is a (ANT_NEXI_term_ant **)
	static int cmp_char_term(const void *a, const void *b);		// a is a (char *) and b is a (ANT_NEXI_term_ant **)

public:
	ANT_snippet(unsigned long max_snippet_length, long length_of_longest_document);
	virtual ~ANT_snippet();

	virtual char *get_snippet(char *snippet, char *document, char *query) = 0;
} ;

#endif /* SNIPPET_H_ */
