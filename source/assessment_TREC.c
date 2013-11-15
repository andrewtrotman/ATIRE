/*
	ASSESSMENT_TREC.C
	-----------------
	This code reads assessment files in the TREC native format
	That format is:
	TopicID (0|SubtopicID) DocID Rel
*/
#include <string.h>
#include "hash_table.h"
#include "assessment_TREC.h"
#include "relevant_document.h"
#include "memory.h"
#include "disk.h"
#include "str.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_ASSESSMENT_TREC::READ()
	---------------------------
*/
ANT_relevant_document *ANT_assessment_TREC::read(char *filename, long long *judgements)
{
char ***found, *pointer_to_document, **pointer_to_pointer_to_document, document[128];		// Assume document IDs are going to be less than 128 characters
char *file, **lines, **current;
long topic, subtopic, relevance, relevance_judgements;
long long lines_in_file;
ANT_relevant_document *current_assessment, *all_assessments;
long params, missing_warned = FALSE;

/*
	load the assessment file into memory
*/
if ((file = ANT_disk::read_entire_file(filename)) == NULL)
	return NULL;
lines = ANT_disk::buffer_to_list(file, &lines_in_file);

/*
	count the number of relevance judgements
*/
relevance_judgements = 0;
for (current = lines; *current != 0; current++)
	{
	params = sscanf(*current, "%ld %ld %127s %ld", &topic, &subtopic, document, &relevance);
	document[127] = '\0';
	if (params == 4)
		relevance_judgements++;
	}

/*
	allocate space for the assessments
*/
current_assessment = all_assessments = (ANT_relevant_document *)memory->malloc(sizeof(*all_assessments) * relevance_judgements);

/*
	generate the list of relevance judgements
*/
for (current = lines; *current != 0; current++)
	{
	params = sscanf(*current, "%ld %ld %127s %ld", &topic, &subtopic, document, &relevance);
	document[127] = '\0';
	if (params >= 4)
		{
		pointer_to_document = (char *)document;
		pointer_to_pointer_to_document = &pointer_to_document;
		found = (char ***)bsearch(&pointer_to_pointer_to_document, sorted_docid_list, (size_t)documents, sizeof(*sorted_docid_list), char_star_star_star_strcmp);
		if (found == NULL)
			{
			if (!missing_warned)
				printf("Warning: DOC '%s' is in the assessments, but not in the collection (now surpressing this warning)\n", document);
			/*
				Include it as a document with a _bad_ id that would never
				exist in the collection

				So, we can't just drop assessments for documents that aren't in the collection becasuse of spam filtering.
				If we drop a relevant document in the spam filter then that should not affect the ideal gain vector in DCG
				so each must have a unique id and be un-findable.  We achieve this by hashing the document's (TREC) ID and
				making that negative so that it does not clash with true internal docids.

				Matt Crane made did this.  It might result in a hash collision and so I've asked him to fix it.
			*/
			current_assessment->docid = -((long long)ANT_random_hash_24(document, strlen(document)));
			missing_warned = TRUE;
			}
		else
			current_assessment->docid = *found - docid_list;		// the position in the list of documents is the internal docid used for computing precision

		current_assessment->topic = topic;
		current_assessment->subtopic = subtopic;
		current_assessment->document_length = relevance;
		current_assessment->relevant_characters = relevance;
		current_assessment->passage_list = NULL;
		current_assessment->number_of_relevant_passages = 0;
		current_assessment->best_entry_point = 0;
		current_assessment++;
		}
	}

/*
	clean up
*/
delete [] file;
delete [] lines;

/*
	and return
*/
*judgements = relevance_judgements;
return all_assessments;
}

