/*
	SEARCH_ENGINE_FORUM_INEX.C
	--------------------------
*/
#include "pragma.h"
#include "search_engine_forum_INEX.h"
#include "focus_result.h"
#include "unicode.h"

/*
	ANT_SEARCH_ENGINE_FORUM_INEX::ANT_SEARCH_ENGINE_FORUM_INEX()
	------------------------------------------------------------
*/
ANT_search_engine_forum_INEX::ANT_search_engine_forum_INEX(char *filename, char *participant_id, char *run_id, char *task) : ANT_search_engine_forum(filename)
{
fprintf(file, "<inex-submission participant-id=\"%s\" run-id=\"%s\" task=\"%s\" query=\"automatic\" result-type=\"element\">\n", participant_id, run_id, task);
fprintf(file, "<topic-fields title=\"yes\" castitle=\"no\" description=\"no\" narrative=\"no\"/>\n");
fprintf(file, "<description>None</description>\n");
fprintf(file, "<collections>\n<collection>wikipedia</collection>\n</collections>\n");
}

/*
	ANT_SEARCH_ENGINE_FORUM_INEX::~ANT_SEARCH_ENGINE_FORUM_INEX()
	-------------------------------------------------------------
*/
ANT_search_engine_forum_INEX::~ANT_search_engine_forum_INEX()
{
fprintf(file, "</inex-submission>");
}

/*
	ANT_SEARCH_ENGINE_FORUM_INEX::FOCUS_TO_INEX()
	---------------------------------------------
	Given the document and a the start and end pointers
	generate the INEX offsets for the pointers.  INEX counts
	in Unicode characters excluding XML tags
*/
ANT_focus_result *ANT_search_engine_forum_INEX::focus_to_INEX(char *document, ANT_focus_result *result)
{
long long offset;
char *current;
long bytes;

result->INEX_start = result->INEX_finish = offset = 0;
current = document;

for (current = document; *current != '\0'; current++)
	if (*current == '<' && *(current + 1) != '!' && *(current + 1) != '?')
		break;

while (*current != '\0')
	{
	if (current < result->start)
		result->INEX_start = offset;
	if (current < result->finish)
		result->INEX_finish = offset;

	if (*current == '<')
		{
		/*
			XML Tags
		*/
		while (*current != '>' && *current != '\0')
			current++;

		if (*current != '\0')
			current++;
		}
	else if (*current == '&')
		{
		/*
			XML Entity References
		*/
		while (*current != ' ' && *current != ';' && *current != '\0')
			current++;

		offset++;
		if (*current != '\0')
			current++;
		}
	else
		{
		bytes = utf8_bytes(current);
		offset++;

		/*
			We do this the long way incase we get a high but set character as the 
			last character of the file and it isn't a UTF-8 formatted document.
		*/
		while (bytes > 0)
			{
			bytes--;
			if (*current++ == '\0')
				return result;
			}
		}
	}

return result;
}

/*
	ANT_SEARCH_ENGINE_FORUM_INEX::WRITE()
	-------------------------------------
*/
void ANT_search_engine_forum_INEX::write(long topic_id, char **docids, long long hits, ANT_search_engine *search_engine, ANT_focus_results_list *focused_results)
{
long long which;
fprintf(file, "<topic topic-id=\"%ld\">\n", topic_id);

for (which = 0; which < hits; which++)
	{
	fprintf(file, "<result>\n");
	fprintf(file, "<file>%s</file>\n", docids[which]);
	fprintf(file, "<path>/article[1]</path>\n");
	fprintf(file, "<rank>%lld</rank>", which);
	fprintf(file, "</result>\n");
	}

fprintf(file, "</topic>\n");
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
