/*
	ASSESSMENT_ANT.C
	----------------
*/
#include <stdio.h>
#include "disk.h"
#include "assessment_ANT.h"
#include "memory.h"
#include "relevant_document.h"

/*
	ANT_ASSESSMENT_ANT::READ()
	--------------------------
*/
ANT_relevant_document *ANT_assessment_ANT::read(char *filename, long long *reldocs)
{
ANT_relevant_document *current_assessment, *all_assessments;
long long lines_in_file;
char *file, **lines, **current;

if ((file = ANT_disk::read_entire_file(filename)) == NULL)
	return NULL;
lines = ANT_disk::buffer_to_list(file, &lines_in_file);

all_assessments = current_assessment = (ANT_relevant_document *)memory->malloc(sizeof(*all_assessments) * lines_in_file);

for (current = lines; *current != NULL; current++)
	{
	if (**current == '\0')
		continue;			// blank line
	if ((sscanf(*current, "%ld %lld", &current_assessment->topic, &current_assessment->docid)) != 2)
		exit(printf("%s line %d:Cannot extract '<queryid> <docid>'\n", filename, current_assessment - all_assessments));
	current_assessment->document_length = current_assessment->relevant_characters = 1024;		// fake the length and proportion of the document that are relevant
	current_assessment++;
	}

delete [] file;
delete [] lines;

*reldocs = lines_in_file;
return all_assessments;
}

