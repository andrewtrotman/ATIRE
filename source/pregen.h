#ifndef PREGEN_H_
#define PREGEN_H_

#include <utility>

#include "fundamental_types.h"

#include "file.h"
#include "search_engine_accumulator.h"
#include "string_pair.h"
#include "str.h"

#define MAX_PREGEN_FIELDS 128
#define PREGEN_FILE_VERSION 1

enum pregen_field_type { INTEGER, STRTRUNC, ASCIIDIGEST, BASE36, RECENTDATE, INTEGEREXACT, STREXACT };

typedef ANT_search_engine_accumulator::ANT_accumulator_t pregen_t;

struct pregen_file_header
{
	uint32_t version;
	uint32_t doc_count;
	uint32_t field_type;
	uint32_t field_name_length;
};

class ANT_pregen
{
protected:
	ANT_file file;

public:
	char *field_name;
	pregen_field_type type;

	pregen_t *scores;
	uint32_t doc_count;

	ANT_pregen() : field_name(NULL), scores(NULL) {};
	ANT_pregen(pregen_field_type type, const char * name);
	virtual ~ANT_pregen();

	int read(const char * filename);
};


class ANT_pregen_writer
{
friend class ANT_pregens_writer;

protected:
	ANT_file file;

public:
	char *field_name;
	pregen_field_type type;

	uint32_t doc_count;

	ANT_pregen_writer() : field_name(NULL) {};
	ANT_pregen_writer(pregen_field_type type, const char * name);
	virtual ~ANT_pregen_writer();

	virtual int open_write(const char * filename) = 0;
	virtual void close_write() = 0;

	virtual void add_field(long long docindex, ANT_string_pair & content) = 0;
};

/**
 * This pregen field class is for the normal case where the score for a document pregen
 * field can be computed immediately when it is first encountered.
 */
class ANT_pregen_writer_normal : public ANT_pregen_writer
{
private:
	static pregen_t generate_integer(ANT_string_pair field);
	static pregen_t generate_strtrunc(ANT_string_pair field);
	static pregen_t generate_asciidigest(ANT_string_pair field);
	static pregen_t generate_base36(ANT_string_pair field);
	static pregen_t generate_recentdate(ANT_string_pair field);

	static pregen_t generate(pregen_field_type type, ANT_string_pair field);

	void add_score(pregen_t score, long long count = 1);
public:
	ANT_pregen_writer_normal(pregen_field_type type, const char * name) : ANT_pregen_writer(type, name) {};
	ANT_pregen_writer_normal() : ANT_pregen_writer() {};
	virtual ~ANT_pregen_writer_normal() {};

	virtual void add_field(long long docindex, ANT_string_pair & content);

	virtual int open_write(const char * filename);
	virtual void close_write();
};

/**
 * An exact field collects all of the pregen values during indexing, then as a final
 * process when the pregen file is closed, it sorts every value collected and sets the RSV
 * to the document's position in the sorted order.
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

	virtual void add_field(long long docindex, ANT_string_pair & content);

	virtual int open_write(const char * filename);
	virtual void close_write();
};

/**
 * An exact field collects all of the pregen values during indexing, then as a final
 * process when the pregen file is closed, it sorts every value collected and sets the RSV
 * to the document's position in the sorted order.
 */
class ANT_pregen_writer_exact_strings : public ANT_pregen_writer
{
private:
	std::pair<long long, char *> *exact_strings;

	uint32_t doc_capacity;

	void ensure_storage();
	void add_exact_string(char *str);
public:
	ANT_pregen_writer_exact_strings(const char * name);
	ANT_pregen_writer_exact_strings() : ANT_pregen_writer() {};
	virtual ~ANT_pregen_writer_exact_strings();

	virtual void print_strings();

	virtual void add_field(long long docindex, ANT_string_pair & content);

	virtual int open_write(const char * filename);
	virtual void close_write();
};

/**
 * Generates pregens for a number of different fields on a document collection simultaneously.
 */
class ANT_pregens_writer
{
public:
	ANT_pregen_writer *fields[MAX_PREGEN_FIELDS];
	int field_count;

	ANT_pregens_writer();
	virtual ~ANT_pregens_writer();

	int add_field(const char * filename, const char * field_name, pregen_field_type type);

	void process_document(long long doc_index, char * doc_name);

	void close();
};

#endif
