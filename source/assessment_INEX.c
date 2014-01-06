/*
	ASSESSMENT_INEX.C
	-----------------
	This code reads assessment files in the INEX 2008 native format
	That format is:
	TopicID Q0 DocID RelBytes DocLength [BEPOffset [Offset:Length]]
*/
#include <string.h>
#include "assessment_TREC.h"
#include "assessment_INEX.h"
#include "relevant_document.h"
#include "relevant_document_passage.h"
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
	ANT_ASSESSMENT_INEX::READ()
	---------------------------
*/
ANT_relevant_document *ANT_assessment_INEX::read(char *filename, long long *judgements)
{
char *file, **lines, **current, *space;
#ifdef FILENAME_INDEX
	long topic, document_length, relevant_characters, relevant_documents, relevant_passages, best_entry_point;
	char document[128];
#else
	long topic, document, document_length, relevant_characters, relevant_documents, relevant_passages, best_entry_point, *document_pointer, **found;
	long missing_warned = FALSE;
#endif
long long lines_in_file;
ANT_relevant_document *current_assessment, *all_assessments;
ANT_relevant_document_passage *current_passage, *all_passages, *passage_list_for_this_document;
long params, length_warned = FALSE;

/*
	load the assessment file into memory
*/
if ((file = ANT_assessment_TREC::read_entire_file(filename)) == NULL)
	return NULL;
lines = ANT_disk::buffer_to_list(file, &lines_in_file);

/*
	count the number of relevant documents
*/
relevant_documents = 0;
relevant_passages = 0;
for (current = lines; *current != 0; current++)
	{
	#ifdef FILENAME_INDEX
		params = sscanf(*current, "%ld %*s %127s %ld %ld", &topic, document, &relevant_characters, &document_length);
	#else
		params = sscanf(*current, "%ld %*s %ld %ld %ld", &topic, &document, &relevant_characters, &document_length);
	#endif
	relevant_passages += strcountchr(*current, ':');
	if (params == 4)
		relevant_documents++;
	}

/*
	allocate space for the assessments
*/
current_assessment = all_assessments = (ANT_relevant_document *)memory->malloc(sizeof(*all_assessments) * relevant_documents);
current_passage = all_passages = (ANT_relevant_document_passage *)memory->malloc(sizeof(*all_passages) * relevant_passages);

/*
	generate the list of relevant documents
*/
#ifndef FILENAME_INDEX
	document_pointer = &document;
#endif
for (current = lines; *current != 0; current++)
	{
	best_entry_point = 0;
	#ifdef FILENAME_INDEX
		params = sscanf(*current, "%ld %*s %127s %ld %ld %ld", &topic, document, &relevant_characters, &document_length, &best_entry_point);
		document[127] = '\0';
	#else
		params = sscanf(*current, "%ld %*s %ld %ld %ld %ld", &topic, &document, &relevant_characters, &document_length, &best_entry_point);
	#endif
	if (params >= 4)
		{
		#ifdef FILENAME_INDEX
			current_assessment->docid = strnew(document);
		#else
			found = (long **)bsearch(&document_pointer, sorted_numeric_docid_list, (size_t)documents, sizeof(*sorted_numeric_docid_list), cmp);
			if (found == NULL)
				{
				if (!missing_warned)
					printf("Warning: DOC '%ld' is in the assessments, but not in the collection (now surpressing this warning)\n", document);
				missing_warned = TRUE;
				}
			else
				current_assessment->docid = *found - numeric_docid_list;		// the position in the list of documents is the internal docid used for computing precision
		#endif
		current_assessment->topic = topic;
		current_assessment->subtopic = 0; // for now INEX doesn't give subtopics
		if (relevant_characters > document_length)
			{
			if (!length_warned)
				fprintf(stderr, "Document:%lld (topic:%lld) has more relevant characters than the length of the document (%lld vs %lld) (now surpressing this warning)\n", (long long)current_assessment->docid, (long long)current_assessment->topic, (long long)relevant_characters, (long long)document_length);
			relevant_characters = document_length;			// recover by setting the amount of relevant material to the length of the document (ie 100% precision).
			length_warned = TRUE;
			}
		/*
			Find each relevant passage and build the chain
		*/
		passage_list_for_this_document = NULL;
		if (relevant_characters != 0)
			{
			passage_list_for_this_document = current_passage;
			space = strchr(*current, ':');
			while (*space != ' ')
				space--;
			space = strpbrk(space, "1234567890");
			do
				{
				params = sscanf(space, "%lld:%lld", &current_passage->offset, &current_passage->length);
				current_passage->next = current_passage + 1;
				current_passage++;
				if ((space = strchr(space, ' ')) != NULL)
					space = strpbrk(space, "1234567890");
				}
			while (space != NULL);
			(current_passage - 1)->next = NULL;		// NULL terminate the end of the previous list
			}

		current_assessment->document_length = document_length;
		current_assessment->relevant_characters = relevant_characters;
		current_assessment->passage_list = passage_list_for_this_document;
		current_assessment->number_of_relevant_passages = current_passage - passage_list_for_this_document;
		current_assessment->best_entry_point = best_entry_point;
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
*judgements = relevant_documents;
return all_assessments;
}

