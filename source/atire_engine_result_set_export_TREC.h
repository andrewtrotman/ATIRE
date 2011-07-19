/*
	ATIRE_ENGINE_RESULT_SET_EXPORT_TREC.H
	-------------------------------------
*/
#ifndef ATIRE_ENGINE_RESULT_SET_EXPORT_TREC_H_
#define ATIRE_ENGINE_RESULT_SET_EXPORT_TREC_H_

#include "atire_engine_result_set_export.h"

/*
	class ATIRE_ENGINE_RESULT_SET_EXPORT_TREC
	-----------------------------------------
*/
class ATIRE_engine_result_set_export_TREC : public ATIRE_engine_result_set_export
{
private:
	char *run_name;

public:
	ATIRE_engine_result_set_export_TREC(char *run_name);
	virtual ~ATIRE_engine_result_set_export_TREC();

	virtual void preamble(void) {}
	virtual void include(long long topic_id, ATIRE_engine_result_set *results, long long first, long long page_length);
	virtual void postamble(void) {}
} ;


#endif /* ATIRE_ENGINE_RESULT_SET_EXPORT_TREC_H_ */
