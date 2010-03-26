/*
	FOCUS_RESULT.H
	--------------
*/
#ifndef FOCUS_RESULT_H_
#define FOCUS_RESULT_H_

#include "search_engine_accumulator.h"

/*
	class ANT_FOCUS_RESULT
	----------------------
*/
class ANT_focus_result
{
friend class ANT_focus_results_list;

public:
	long long docid;											// the search engine's id for the document
	char *document_name;										// the unique name of the document (e.g. the TREC ID)
	ANT_search_engine_accumulator::ANT_accumulator_t rsv;		// rsv of the passage (using what ever accumulator type the search engine uses)
	char *start, *finish;										// passage as pointers into the document
	long long INEX_start, INEX_finish;							// passage as INEX offsets into the document

	void clear_rsv(void) { rsv = 0; }
	void set_rsv(ANT_search_engine_accumulator::ANT_accumulator_t score) { rsv = score; }
	ANT_search_engine_accumulator::ANT_accumulator_t get_rsv(void) { return rsv; }
} ;

#endif /* FOCUS_RESULT_H_ */
