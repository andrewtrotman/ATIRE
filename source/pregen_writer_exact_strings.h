/*
	PREGEN_WRITER_EXACT_STRINGS.H
	-----------------------------
*/
#ifndef PREGEN_WRITER_EXACT_STRINGS_H_
#define PREGEN_WRITER_EXACT_STRINGS_H_

#include <utility>
#include "pregen_writer.h"

/*
	class ANT_PREGEN_WRITER_EXACT_STRINGS
	-------------------------------------
	An exact field collects all of the pregen values during indexing, then as a final
	process when the pregen file is closed, it sorts every value collected and sets the RSV
	to the document's position in the sorted order.
*/
class ANT_pregen_writer_exact_strings : public ANT_pregen_writer
{
private:
	int restricted; //If we're comparing based on a restricted character set (e.g. throw away punctuation)

	ANT_memory memory;
	std::pair<long long, ANT_string_pair> *exact_strings;

	uint32_t doc_capacity;

private:
	void ensure_storage();
	void add_exact_string(ANT_string_pair string);

public:
	ANT_pregen_writer_exact_strings(const char * name, int restricted);
	virtual ~ANT_pregen_writer_exact_strings();

	virtual void add_field(long long docindex, ANT_string_pair content);

	virtual void close_write();
} ;

#endif /* PREGEN_WRITER_EXACT_STRINGS_H_ */
