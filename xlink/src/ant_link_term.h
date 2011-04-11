/*
 * ant_link_term.h
 *
 *  Created on: Sep 22, 2009
 *      Author: monfee
 */

#ifndef ANT_LINK_TERM_H_
#define ANT_LINK_TERM_H_

#include <string.h>
#include <vector>

class ANT_link_posting;

class ANT_link_term
{
public:
	char *term;
	//long postings_length;			// which is also the number of documents pointed to.
	std::vector<ANT_link_posting *>	postings;
	long total_occurences;			// number of times the phrase occurs as a link in the collection
	long collection_frequency;		// number of times the phrase occurs in the colleciton
	long document_frequency;		// number of documents in which the phrase occurs
public:
	ANT_link_term();
	~ANT_link_term();

	static int compare(const void *a, const void *b);

	bool operator==(const ANT_link_term& rhs);
	bool operator==(const char *term);
} ;

class ANT_link_term_compare {
public:
	const bool operator()(const ANT_link_term *a, const ANT_link_term * b) const;
};

class term_part_equal {
private:
	const char *term_to_find;

public:
	term_part_equal(const char *term) : term_to_find(term) {}
	~term_part_equal() {}

	bool operator()(ANT_link_term* other) const
	{
		return strncmp(other->term, term_to_find, strlen(term_to_find)) == 0;
	}
};

#endif /* ANT_LINK_TERM_H_ */
