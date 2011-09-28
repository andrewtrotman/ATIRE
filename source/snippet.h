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
class ANT_search_engine;

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
	char query_buffer[MAX_TERM_LENGTH];			// this is used as a buffer to store the query terms in the case where they are stemmed.  It avoids trashing the original query string
	char *document_text;						// a buffer large enough to hold the document's text once the XML tags have been removed.
	ANT_NEXI_term_ant **term_list;				// the most recently parsed query (once parsed)
	long terms_in_query;						// the number of terms in the most recently parsed query
	ANT_stem *stemmer;							// the stemming algorithm
	ANT_search_engine *engine;					// the search engine used to resolve stems

protected:
	char *next_n_characters_after(char *snippet, long maximum_snippet_length, char *starting_point = NULL);
	char *XML_to_text(char *destination, char *source);
	static int cmp_term(const void *a, const void *b);		// a is a (ANT_parser_token *) and b is a (ANT_NEXI_term_ant **)
	static int cmp_char_term(const void *a, const void *b);		// a is a (char *) and b is a (ANT_NEXI_term_ant **)

public:
	ANT_snippet(unsigned long max_snippet_length, long length_of_longest_document, ANT_search_engine *engine, ANT_stem *stemmer);
	virtual ~ANT_snippet();

	virtual ANT_NEXI_term_ant **parse_query(char *query);
	virtual char *get_snippet(char *snippet, char *document) = 0;
} ;

#endif /* SNIPPET_H_ */
