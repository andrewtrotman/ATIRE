/*
	PREGEN_WRITER.H
	---------------
*/
#ifndef PREGEN_WRITER_H_
#define PREGEN_WRITER_H_

#include "fundamental_types.h"
#include "file.h"
#include "pregen_field_type.h"
#include "string_pair.h"

/*
	class ANT_PREGEN_WRITER
	-----------------------
*/
class ANT_pregen_writer
{
friend class ANT_pregens_writer;

protected:
	ANT_file file;

public:
	char *field_name;
	ANT_pregen_field_type type;
	uint32_t doc_count;

public:
	ANT_pregen_writer() : field_name(NULL) {};
	ANT_pregen_writer(ANT_pregen_field_type type, const char *name);
	virtual ~ANT_pregen_writer()	{ delete [] field_name; }

	virtual int open_write(const char * filename);
	virtual void close_write();

	virtual void add_field(long long docindex, ANT_string_pair content) = 0;
};

#endif /* PREGEN_WRITER_H_ */
