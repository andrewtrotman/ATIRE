/*
	ATIRE_ENGINE_RESULT_SET_EXPORT_TREC.C
	-------------------------------------
*/
#include "atire_engine_result.h"
#include "atire_engine_result_set.h"
#include "atire_engine_result_set_export_TREC.h"

/*
	ATIRE_ENGINE_RESULT_SET_EXPORT_TREC::ATIRE_ENGINE_RESULT_SET_EXPORT_TREC()
	--------------------------------------------------------------------------
*/
ATIRE_engine_result_set_export_TREC::ATIRE_engine_result_set_export_TREC(char *run_name) : ATIRE_engine_result_set_export()
{
this->run_name = run_name == NULL ? strnew("Unknown") : strnew(run_name);
}

/*
	ATIRE_ENGINE_RESULT_SET_EXPORT_TREC::~ATIRE_ENGINE_RESULT_SET_EXPORT_TREC()
	---------------------------------------------------------------------------
*/
ATIRE_engine_result_set_export_TREC::~ATIRE_engine_result_set_export_TREC()
{
delete [] run_name;
}

/*
	ATIRE_ENGINE_RESULT_SET_EXPORT_TREC::INCLUDE()
	----------------------------------------------
*/
void ATIRE_engine_result_set_export_TREC::include(long long topic_id, ATIRE_engine_result_set *results_list, long long first, long long page_length)
{
long long current, from, to;

if (first < results_list->hits)
	{
	results_list->sort();

	from = first - 1;
	to = from + page_length < results_list->hits ? from + page_length : results_list->hits;
	for (current = from; current < to; current++)
		{
		result << topic_id << " ";
		result << "Q0" << " ";
		result << results_list->results[current].name << " ";
		result << current + 1 << " ";
		result << results_list->results[current].rsv << " ";
		result << run_name << std::endl;
		}
	}
}

