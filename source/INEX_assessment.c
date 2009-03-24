/*
	INEX_ASSESSMENT.C
	-----------------
*/
#include <string.h>
#include "INEX_assessment.h"
#include "relevant_document.h"
#include "memory.h"
#include "disk.h"

/*
	ANT_INEX_ASSESSMENT::CMP()
	--------------------------
*/
int ANT_INEX_assessment::cmp(const void *a, const void *b)
{
long **one, **two;

one = (long **)a;
two = (long **)b;

return **one - **two;
}

/*
	ANT_INEX_ASSESSMENT::MAX()
	--------------------------
*/
inline char *ANT_INEX_assessment::max(char *a, char *b, char *c)
{
char *thus_far;

thus_far = a;
if (b > thus_far)
	thus_far = b;
if (c > thus_far)
	thus_far = c;

return thus_far;
}

/*
	ANT_INEX_ASSESSMENT::ANT_INEX_ASSESSMENT()
	------------------------------------------
*/
ANT_INEX_assessment::ANT_INEX_assessment(ANT_memory *mem, char **docid_list, long long documents)
{
char **current, *slish, *slosh, *slash, *start;
long *current_docid, **current_sorted_docid;

memory = mem;
this->documents = documents;
current_docid = numeric_docid_list = (long *)memory->malloc(sizeof(*numeric_docid_list) * documents);
current_sorted_docid = sorted_numeric_docid_list = (long **)memory->malloc(sizeof(*sorted_numeric_docid_list) * documents);
for (current = docid_list; *current != NULL; current++)
	{
	slish = *current;
	slash = strchr(*current, '/');
	slosh = strchr(*current, '\\');
	start = max(slish, slash, slosh);		// get the posn of the final dir seperator (or the start of the string)
	if (*start != '\0')		// avoid blank lines at the end of the file
		{
		*current_docid = atol(start + (start == slish ? 0 : 1));
		*current_sorted_docid = current_docid;
		current_docid++;
		current_sorted_docid++;
		}
	}
qsort(sorted_numeric_docid_list, (size_t)documents, sizeof(*sorted_numeric_docid_list), cmp);
}

/*
	ANT_INEX_ASSESSMENT::READ()
	---------------------------
*/
ANT_relevant_document *ANT_INEX_assessment::read(char *filename, long long *reldocs)
{
ANT_disk disk;
char *file, **lines, **current;
long topic, document, document_length, relevant_characters, relevant_documents, *document_pointer, **found;
long long lines_in_file;
ANT_relevant_document *current_assessment, *all_assessments;
long params;

/*
	load the assessment file into memory
*/
if ((file = disk.read_entire_file(filename)) == NULL)
	return NULL;
lines = disk.buffer_to_list(file, &lines_in_file);

/*
	count the number of relevant documents
*/
relevant_documents = 0;
for (current = lines; *current != 0; current++)
	{
	params = sscanf(*current, "%ld %*s %ld %ld %ld", &topic, &document, &relevant_characters, &document_length);
	if ((params == 4) && (relevant_characters != 0))
		relevant_documents++;
	}

/*
	allocate space for the (positive) assessments
*/
current_assessment = all_assessments = (ANT_relevant_document *)memory->malloc(sizeof(*all_assessments) * relevant_documents);

/*
	generate the list of relevant documents
*/
document_pointer = &document;
for (current = lines; *current != 0; current++)
	{
	params = sscanf(*current, "%ld %*s %ld %ld %ld", &topic, &document, &relevant_characters, &document_length);
	if ((params == 4) && (relevant_characters != 0))
		{
		found = (long **)bsearch(&document_pointer, sorted_numeric_docid_list, (size_t)documents, sizeof(*sorted_numeric_docid_list), cmp);
		if (found == NULL)
			printf("DOC:%ld is in the assessments, but not in the collection\n", document);
		else
			current_assessment->docid = *found - numeric_docid_list;		// the position in the list of documents is the internal docid used for computing precision
		current_assessment->topic = topic;
		if (relevant_characters > document_length)
			{
			fprintf(stderr, "Document:%lld (topic:%lld) has more relevant characters than the length of the document (%lld vs %lld)\n", (long long)current_assessment->docid, (long long)current_assessment->topic, (long long)relevant_characters, (long long)document_length);
			relevant_characters = document_length;			// recover by setting the amount of relevant material to the length of the document (ie 100% precision).
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

