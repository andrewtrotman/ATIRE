/*
	ATIRE_ENGINE_RESULT_SET_EXPORT_INEX_SNIPPET.C
	---------------------------------------------
*/
#include "atire_engine_result.h"
#include "atire_engine_result_set.h"
#include "atire_engine_result_set_export_INEX_snippet.h"

/*
	ATIRE_ENGINE_RESULT_SET_EXPORT_INEX_SNIPPET::ATIRE_ENGINE_RESULT_SET_EXPORT_INEX_SNIPPET()
	------------------------------------------------------------------------------------------
*/
ATIRE_engine_result_set_export_INEX_snippet::ATIRE_engine_result_set_export_INEX_snippet(char *group_name, char *run_name, char *description) : ATIRE_engine_result_set_export()
{
this->group_name = group_name == NULL ? strnew("Unknown") : strnew(group_name);
this->run_name = run_name == NULL ? strnew("Unknown") : strnew(run_name);
this->description = description == NULL ? strnew("Unknown") : strnew(description);
}

/*
	ATIRE_ENGINE_RESULT_SET_EXPORT_INEX_SNIPPET::~ATIRE_ENGINE_RESULT_SET_EXPORT_INEX_SNIPPET()
	-------------------------------------------------------------------------------------------
*/
ATIRE_engine_result_set_export_INEX_snippet::~ATIRE_engine_result_set_export_INEX_snippet()
{
delete [] group_name;
delete [] run_name;
delete [] description;
}

/*
	ATIRE_ENGINE_RESULT_SET_EXPORT_INEX_SNIPPET::PREAMBLE()
	-------------------------------------------------------
*/
void ATIRE_engine_result_set_export_INEX_snippet::preamble(void)
{
result << "<inex-snippet-submission participant-id=" << '"' << group_name << '"' << " run-id=" << '"' << run_name << '"' << ">" << std::endl;
result << "<description>" << description << "</description>" << std::endl;
}

/*
	ATIRE_ENGINE_RESULT_SET_EXPORT_INEX_SNIPPET::INCLUDE()
	------------------------------------------------------
*/
void ATIRE_engine_result_set_export_INEX_snippet::include(long long topic_id, ATIRE_engine_result_set *results_list, long long first, long long page_length)
{
long long current, from, to;

if (first < results_list->hits)
	{
	result << "<topic topic-id=" << '"' << topic_id << '"' << ">" << std::endl;
	results_list->sort();

	from = first - 1;
	to = from + page_length < results_list->hits ? from + page_length : results_list->hits;
	for (current = from; current < to; current++)
		{
		result << "   <snippet doc-id=" << '"' << results_list->results[current].name << '"';
		result << " rsv=" << '"' << results_list->results[current].rsv << '"' << '>';
		result << "</snippet>" << std::endl;
		}
	result << "</topic>" << std::endl;
	}
}

/*
	ATIRE_ENGINE_RESULT_SET_EXPORT_INEX_SNIPPET::POSTAMBLE()
	--------------------------------------------------------
*/
void ATIRE_engine_result_set_export_INEX_snippet::postamble(void)
{
result << "</inex-snippet-submission>" << std::endl;
}
