#ifndef PREGEN_H_
#define PREGEN_H_

#include "fundamental_types.h"

#include "file.h"
#include "search_engine_accumulator.h"
#include "string_pair.h"
#include "str.h"

#define MAX_PREGEN_FIELDS 128
#define PREGEN_FILE_VERSION 1

enum pregen_field_type { INTEGER, STRTRUNC, STREXACT, ASCIIDIGEST, BASE36, RECENTDATE };

typedef ANT_search_engine_accumulator::ANT_accumulator_t pregen_t;

class ANT_pregen_field
{
friend class ANT_pregen;

private:
	struct file_header
	{
		uint32_t version;
		uint32_t doc_count;
		uint32_t field_type;
		uint32_t field_name_length;
	};

	ANT_file file;

public:
	char *field_name;
	pregen_field_type type;

	pregen_t *scores;
	uint32_t doc_count;

	void add_score(pregen_t score, long long count = 1);

	int read(const char * filename);

	int open_write(const char * filename);
	void close_write();

	ANT_pregen_field() : field_name(NULL), scores(NULL) {};
	ANT_pregen_field(pregen_field_type type, const char * name);

	~ANT_pregen_field();
};

class ANT_pregen
{
private:
	void process_field(long long docindex, ANT_pregen_field & field, ANT_string_pair & content);

	static pregen_t generate_integer(ANT_string_pair field);
	static pregen_t generate_strtrunc(ANT_string_pair field);
	static pregen_t generate_asciidigest(ANT_string_pair field);
	static pregen_t generate_base36(ANT_string_pair field);
	static pregen_t generate_recentdate(ANT_string_pair field);
	static pregen_t generate(pregen_field_type type, ANT_string_pair field);

public:
	ANT_pregen_field *fields[MAX_PREGEN_FIELDS];
	int field_count;

	ANT_pregen();
	virtual ~ANT_pregen();

	int add_field(char * filename, const char * field_name, pregen_field_type type);

	void process_document(long long doc_index, char * doc_name);

	void close();
};

#endif
