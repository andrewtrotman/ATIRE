/*
	PREGENS_WRITER.H
	----------------
*/
#ifndef PREGENS_WRITER_H_
#define PREGENS_WRITER_H_

#include "pregen_writer.h"

#define MAX_PREGEN_FIELDS 128

/*
	class ANT_PREGENS_WRITER
	------------------------
*/
class ANT_pregens_writer
{
public:
	ANT_pregen_writer *fields[MAX_PREGEN_FIELDS];
	int field_count;

private:
	ANT_string_pair strip_garbage(const ANT_string_pair & s);

public:
	ANT_pregens_writer();
	virtual ~ANT_pregens_writer();

	int add_field(const char *filename, const char *field_name, ANT_pregen_field_type type);

	void process_document(long long doc_index, char *doc_name);
	void process_document(long long doc_index, ANT_string_pair doc_name);

	void close();
} ;



#endif /* PREGENS_WRITER_H_ */
