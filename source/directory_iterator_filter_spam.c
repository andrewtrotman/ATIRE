/*
	DIRECTORY_ITERATOR_FILTER_SPAM.C
	--------------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include "maths.h"
#include "str.h"
#include "disk.h"
#include "directory_iterator_filter_spam.h"

/*
	ANT_DIRECTORY_ITERATOR_FILTER_SPAM::ANT_DIRECTORY_ITERATOR_FILTER_SPAM()
	------------------------------------------------------------------------
	Each line of the file containing spam scores should have the score followed by docid and be sorted by docid
*/
ANT_directory_iterator_filter_spam::ANT_directory_iterator_filter_spam(ANT_directory_iterator *source, char *filename, long long threshold, long get_file) : ANT_directory_iterator_filter(source, get_file)
{
method = threshold < 50 ? EXCLUDE : INCLUDE;

if (docids == NULL)
	{
	char *spam_file = ANT_disk::read_entire_file(filename);
	char *ptr = spam_file, *nl;
	char new_line = strchr(ptr, '\r') > strchr(ptr, '\n') ? '\r' : '\n'; // as long as the file is consistent
	long score;
	long docids_recorded = 0;
	number_docs = 0;

	while (*ptr)
		{
		score = ANT_atol(ptr);
		nl = strchr(ptr, new_line);
		if ((method == INCLUDE && score >= threshold) || (method == EXCLUDE && score < threshold))
			number_docs++;
		ptr = nl + 1; // skip to next line
		}

	docids = new char *[number_docs];
	ptr = spam_file;

	while (docids_recorded < number_docs)
		{
		score = ANT_atol(ptr); // get score
		ptr = strchr(ptr, ' ') + 1; // skip to document
		nl = strchr(ptr, new_line);
		if ((method == INCLUDE && score >= threshold) || (method == EXCLUDE && score < threshold))
			docids[docids_recorded++] = strip_space_inplace(strnnew(ptr, nl - ptr)); // strip_space_inplace because we might have captured a line ending
		ptr = nl + 1; // skip to next line
		}

	/*
		We've finished with the spam rankings, so free up that memory
	*/
	delete [] spam_file;
	}
}
