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
ANT_relevant_document *ANT_assessment_ANT::read(char *filename, long long *judgements)
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
	if ((sscanf(*current, "%lld %lld", &current_assessment->topic, &current_assessment->docid)) != 2)
		exit(printf("%s line %lld:Cannot extract '<queryid> <docid>'\n", filename, (long long)(current_assessment - all_assessments)));
	current_assessment->subtopic = 0; // ANT assessment format doesn't do subtopics(?)
	current_assessment->document_length = current_assessment->relevant_characters = 1024;		// fake the length and proportion of the document that are relevant
	current_assessment->passage_list = NULL;
	current_assessment->number_of_relevant_passages = 0;
	current_assessment->best_entry_point = 0;
	current_assessment++;
	}

delete [] file;
delete [] lines;

*judgements = lines_in_file;
return all_assessments;
}

