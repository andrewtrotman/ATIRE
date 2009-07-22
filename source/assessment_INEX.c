/*
	ASSESSMENT_INEX.C
	-----------------
*/
#include <string.h>
#include "assessment_INEX.h"
#include "relevant_document.h"
#include "memory.h"
#include "disk.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_ASSESSMENT_INEX::READ()
	---------------------------
*/
ANT_relevant_document *ANT_assessment_INEX::read(char *filename, long long *reldocs)
{
char *file, **lines, **current;
long topic, document, document_length, relevant_characters, relevant_documents, *document_pointer, **found;
long long lines_in_file;
ANT_relevant_document *current_assessment, *all_assessments;
long params, missing_warned = FALSE, length_warned = FALSE;

/*
	load the assessment file into memory
*/
if ((file = ANT_disk::read_entire_file(filename)) == NULL)
	return NULL;
lines = ANT_disk::buffer_to_list(file, &lines_in_file);

/*
	count the number of relevant documents
*/
relevant_documents = 0;
for (current = lines; *current != 0; current++)
	{
	params = sscanf(*current, "%ld %*s %ld %ld %ld", &topic, &document, &relevant_characters, &document_length);
	if (params == 4)
		relevant_documents++;
	}

/*
	allocate space for the assessments
*/
current_assessment = all_assessments = (ANT_relevant_document *)memory->malloc(sizeof(*all_assessments) * relevant_documents);

/*
	generate the list of relevant documents
*/
document_pointer = &document;
for (current = lines; *current != 0; current++)
	{
	params = sscanf(*current, "%ld %*s %ld %ld %ld", &topic, &document, &relevant_characters, &document_length);
	if (params == 4)
		{
		found = (long **)bsearch(&document_pointer, sorted_numeric_docid_list, (size_t)documents, sizeof(*sorted_numeric_docid_list), cmp);
		if (found == NULL)
			{
			if (!missing_warned)
				printf("Warning: DOC '%ld' is in the assessments, but not in the collection (now surpressing this warning)\n", document);
			missing_warned = TRUE;
			}
		else
			current_assessment->docid = *found - numeric_docid_list;		// the position in the list of documents is the internal docid used for computing precision
		current_assessment->topic = topic;
		if (relevant_characters > document_length)
			{
			if (!length_warned)
				fprintf(stderr, "Document:%lld (topic:%lld) has more relevant characters than the length of the document (%lld vs %lld) (now surpressing this warning)\n", (long long)current_assessment->docid, (long long)current_assessment->topic, (long long)relevant_characters, (long long)document_length);
			relevant_characters = document_length;			// recover by setting the amount of relevant material to the length of the document (ie 100% precision).
			length_warned = TRUE;
			}
		current_assessment->document_length = document_length;
		current_assessment->relevant_characters = relevant_characters;
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
*reldocs = relevant_documents;
return all_assessments;
}

