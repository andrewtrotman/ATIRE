/*
	ASSESSMENT.C
	------------
*/
#include "str.h"
#include "memory.h"
#include "assessment.h"

/*
	ANT_ASSESSMENT::COPY()
	----------------------
*/
void ANT_assessment::copy(ANT_assessment *what)
{
memory = what->memory;
documents = what->documents;
numeric_docid_list = what->numeric_docid_list;
sorted_numeric_docid_list = what->sorted_numeric_docid_list;
docid_list = what->docid_list;
sorted_docid_list = what->sorted_docid_list;
}

/*
	ANT_ASSESSMENT::CMP()
	---------------------
*/
int ANT_assessment::cmp(const void *a, const void *b)
{
long **one, **two;

one = (long **)a;
two = (long **)b;

return **one - **two;
}

/*
	ANT_ASSESSMENT::MAX()
	---------------------
*/
char *ANT_assessment::max(char *a, char *b, char *c)
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
	ANT_ASSESSMENT::ANT_ASSESSMENT()
	--------------------------------
*/
ANT_assessment::ANT_assessment(ANT_memory *mem, char **docid_list, long long documents)
{
#ifdef FILENAME_INDEX
	memory = mem;
	this->documents = documents;
	this->docid_list = docid_list;
#else
	char ***string, **current, *slish, *slosh, *slash, *start;
	long *current_docid, **current_sorted_docid;

	memory = mem;
	this->documents = documents;
	this->docid_list = docid_list;
	string = sorted_docid_list = (char ***)memory->malloc(sizeof(*sorted_docid_list) * documents);
	for (current = docid_list; *current != NULL; current++)
		*string++ = current;
	qsort(sorted_docid_list, (size_t)documents, sizeof(*sorted_docid_list), char_star_star_star_strcmp);

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
#endif
}

