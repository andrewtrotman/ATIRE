/*
	PREGEN_WRITER_EXACT_INTEGERS.H
	------------------------------
*/
#ifndef PREGEN_WRITER_EXACT_INTEGERS_H_
#define PREGEN_WRITER_EXACT_INTEGERS_H_

#include "pregen_writer.h"

/*
	class ANT_PREGEN_WRITER_EXACT_INTEGERS
	--------------------------------------
	An exact field collects all of the pregen values during indexing, then as a final
	process when the pregen file is closed, it sorts every value collected and sets the RSV
	to the document's position in the sorted order.
*/
class ANT_pregen_writer_exact_integers : public ANT_pregen_writer
{
private:
	std::pair<long long, long long> *exact_integers;

	uint32_t doc_capacity;

	void ensure_storage();
	void add_exact_integer(long long i);

public:
	ANT_pregen_writer_exact_integers(const char * name);
	ANT_pregen_writer_exact_integers() : ANT_pregen_writer() {}
	virtual ~ANT_pregen_writer_exact_integers();

	virtual void add_field(long long docindex, ANT_string_pair content);

	virtual void close_write();
} ;

#endif /* PREGEN_WRITER_EXACT_INTEGERS_H_ */
