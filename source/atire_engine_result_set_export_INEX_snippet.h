/*
	ATIRE_ENGINE_RESULT_SET_EXPORT_INEX_SNIPPET.H
	---------------------------------------------
*/
#ifndef ATIRE_ENGINE_RESULT_SET_EXPORT_INEX_SNIPPET_H_
#define ATIRE_ENGINE_RESULT_SET_EXPORT_INEX_SNIPPET_H_

#include "atire_engine_result_set_export.h"

/*
	class ATIRE_ENGINE_RESULT_SET_EXPORT_INEX_SNIPPET
	-------------------------------------------------
*/
class ATIRE_engine_result_set_export_INEX_snippet : public ATIRE_engine_result_set_export
{
private:
	char *group_name;
	char *run_name;
	char *description;

public:
	ATIRE_engine_result_set_export_INEX_snippet(char *group_name, char *run_name, char *description);
	virtual ~ATIRE_engine_result_set_export_INEX_snippet();

	virtual void preamble(void);
	virtual void include(long long topic_id, ATIRE_engine_result_set *results, long long first, long long page_length);
	virtual void postamble(void);
} ;

#endif /* ATIRE_ENGINE_RESULT_SET_EXPORT_INEX_SNIPPET_H_ */
