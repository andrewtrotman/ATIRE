/*
	ATIRE_ENGINE_RESULT_SET_EXPORT.H
	--------------------------------
*/
#ifndef ATIRE_ENGINE_RESULT_SET_EXPORT_H_
#define ATIRE_ENGINE_RESULT_SET_EXPORT_H_

#include <sstream>
#include <iomanip>
#include "str.h"

class ATIRE_engine_result_set;

/*
	class ATIRE_engine_result_set_export
	------------------------------------
*/
class ATIRE_engine_result_set_export
{
protected:
	std::stringstream result;

public:
	ATIRE_engine_result_set_export() { result << std::fixed << std::setprecision(2); }
	virtual ~ATIRE_engine_result_set_export() {};

	virtual void preamble(void) = 0;
	virtual void include(long long topic_id, ATIRE_engine_result_set *results_list, long long first, long long page_length) = 0;
	virtual void postamble(void) = 0;

	virtual char *serialise(void) { return strnew(result.str().c_str()); }
} ;

#endif /* ATIRE_ENGINE_RESULT_SET_EXPORT_H_ */
