#ifndef PREGEN_H_
#define PREGEN_H_

#include <utility>

#include "fundamental_types.h"

#include "file.h"
#include "search_engine_accumulator.h"
#include "string_pair.h"
#include "str.h"
#include "arithmetic_coding.h"
#include "unicode.h"
#include "unicode_tables.h"

#define MAX_PREGEN_FIELDS 128
#define PREGEN_FILE_VERSION 2

#define CHAR_ENCODE_FAIL 255

enum pregen_field_type { INTEGER, BINTRUNC, STRTRUNC, BASE32, BASE36, BASE37, BASE40, RECENTDATE, INTEGEREXACT, STREXACT, STREXACT_RESTRICTED, BASE37_ARITHMETIC, ASCII_PRINTABLES, ASCII_PRINTABLES_ARITHMETIC};

typedef ANT_PREGEN_T pregen_t;

struct pregen_file_header
{
	uint32_t version;
	uint32_t doc_count;
	uint32_t pregen_t_size;
	uint32_t field_type;
	uint32_t field_name_length;
};

/* Encode a character as a base-36 digit (0-9, a-z), or 255 if there is no
 * mapping for this character */
class ANT_encode_char_base36
{
public:
	static const unsigned int num_symbols = 36;

	static unsigned char encode(unsigned char c)
	{
	if (c >= '0' && c <= '9')
		{
		/* Digits sort first*/
		return c - '0';
		}
	else if (c >= 'a' && c <= 'z')
		{
		/* Letters last */
		return c - 'a' + 10;
		}
	return CHAR_ENCODE_FAIL;
	}
};

/* Encode a character as a base-37 digit (' ', 0-9, a-z), or 255 if there is no
 * mapping for this character */
class ANT_encode_char_base37
{
public:
	static const unsigned int num_symbols = 37;

	static unsigned char encode(unsigned char c)
	{
	if (c == ' ')
		return 0;
	if (c >= '0' && c <= '9')
		{
		/* Digits sort second*/
		return c - '0' + 1;
		}
	else if (c >= 'a' && c <= 'z')
		{
		/* Letters last */
		return c - 'a' + 1 + 10;
		}

	return CHAR_ENCODE_FAIL;
	}
};

/* Encode a character as a base-40 digit (' ', punctuation between ' ' and '0' as one char, 0-9,
 * punctuation between '9' and 'z' as one char, a-z, punctuation after 'z' as one char). */
class ANT_encode_char_base40
{
public:
	static const unsigned int num_symbols = 40;

	static unsigned char encode(unsigned char c)
	{
	if (c == ' ')
		return 0;

	if (c < '0')
		return 1;

	if (c >= '0' && c <= '9')
		return c - '0' + 2;

	if (c < 'a')
		return 2 + 10;

	if (c >= 'a' && c <= 'z')
		return c - 'a' + 2 + 10 + 1;

	if (c > 'z')
		return 2 + 10 + 1 + 26;

	return CHAR_ENCODE_FAIL;
	}
};

/* Encode all printable ASCII characters as distinct codepoints (except uppercase letters,
 * provide lowercased input!), plus encode all Unicode characters as one single
 * codepoint larger than ASCII. */
class ANT_encode_char_printable_ascii
{
public:
	static const unsigned int num_symbols = 70;

	static unsigned char encode(unsigned char c)
	{
	if (c < ' ')
		return CHAR_ENCODE_FAIL;

	//Unicode folds on top of the delete character
	if (c > 0x7F)
		c = 0x7F;

	//Eliminate the hole where uppercase characters used to be
	if (c > 'Z')
		c -= ('Z' - 'A' + 1);

	//Eliminate control characters (everything below space)
	c -= ' ';

	return c;
	}
};

/*
 * Encode ASCII alphanumerics into an 5-bit number, with all punctuation merged to one point,
 * and digits doubling up. All letters are distinct codepoints.
 */
class ANT_encode_char_base32_edges
{
public:
	static const unsigned int num_symbols = 32;

	static unsigned char encode(unsigned char c)
	{
	if (c >= '0' && c <= '9')
		return 1 + ((c - '0') >> 1); //Numbers sort second, but they will have to double-up to fit into 5 encodings

	if (c >= 'a' && c <= 'z')
		return c - 'a' + 5; //Letters sort last

	if (c <= LAST_ASCII_CHAR)
		return 0; //Punctuation sorts first

	//We'll sort Unicode along with Z
	return (unsigned char) (num_symbols - 1);
	}
};

/*
 * Encode ASCII alphanumerics into an 5-bit number, with all punctuation merged to one point,
 * and digits doubling up. All letters are distinct codepoints.
 */
class ANT_encode_char_base32
{
public:
	static const unsigned int num_symbols = 32;

	static unsigned char encode(unsigned char c)
	{
	if (c == ' ')
		return 0;

	if (c >= '0' && c <= '9')
		return 1 + ((c - '0') >> 1); //Numbers sort second, but they will have to double-up to fit into 5 encodings

	if (c >= 'a' && c <= 'z')
		return c - 'a' + 6; //Letters sort last

	return CHAR_ENCODE_FAIL;
	}
};

/*
 * Null-encoding, just passses 8-bit chars straight through.
 */
class ANT_encode_char_8bit
{
public:
	static const unsigned int num_symbols = 256;

	static unsigned char encode(unsigned char c)
	{
	return c;
	}
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
	ANT_arithmetic_model *arithmetic_model;

	template <typename T>
	static pregen_t generate_sliding_radix(ANT_string_pair field);

	static pregen_t generate_integer(ANT_string_pair field);
	static pregen_t generate_bintrunc(ANT_string_pair field);
	static pregen_t generate_recentdate(ANT_string_pair field);

	pregen_t generate(ANT_string_pair field);

	void add_score(pregen_t score, long long count = 1);

public:
	template <typename T>
	static pregen_t generate_radix(ANT_string_pair field);

	template <typename T>
	static pregen_t generate_arithmetic(ANT_string_pair field, ANT_arithmetic_model *model);

	ANT_pregen_writer_normal(pregen_field_type type, const char * name) : ANT_pregen_writer(type, name)
	{
	init_models();
	}

	ANT_pregen_writer_normal() : ANT_pregen_writer()
	{
	init_models();
	}

	virtual ~ANT_pregen_writer_normal()
	{
	delete arithmetic_model;
	};

	void init_models()
	{
	if (type == BASE37_ARITHMETIC)
		{
		static int symbol_frequencies[] =
			{
			1317, 11, 33, 11, 13, 3, 3, 1, 3, 5, 6, 719,
			92, 378, 337, 827, 143, 214, 261, 458, 15, 61, 351, 219,
			510, 591, 330, 8, 593, 451, 585, 138, 68, 221, 26, 158, 6
			};

		arithmetic_model = new ANT_arithmetic_model(ANT_encode_char_base37::num_symbols, symbol_frequencies, 0);
		}
	else if (type == ASCII_PRINTABLES_ARITHMETIC)
		{
		static int symbol_frequencies[] =
			{
			1907, /* Space */
			223, 1, 1, 1, 1, 29, 25, 155, 157, 60, 5, 48, 78, 136, 29, /* Punctuation */
			16, 50, 16, 17,	5, 4, 1, 5, 7, 8, /* 0 - 9 */
			41, 30, 1, 3, 1, 190, 1, 29, 1, 28, 11, 4, 1, /* Punctuation */
			1048, 134, 549, 502, 1204, 205, 306, 384, 669, 21, 88, 515, 333, 737, /* a - n */
			854, 481, 12, 858, 643, 850, 199, 100, 315, 38, 225, 8, /* o - z */
			28, 13, 29, 93, /* Punctuation */
			15 /* Unicode */
			};

		arithmetic_model = new ANT_arithmetic_model(ANT_encode_char_printable_ascii::num_symbols, symbol_frequencies, 0);
		}
	else
		arithmetic_model = NULL;
	}

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
	int restricted; //If we're comparing based on a restricted character set (e.g. throw away punctuation)

	ANT_memory memory;
	std::pair<long long, ANT_string_pair> *exact_strings;

	uint32_t doc_capacity;

	void ensure_storage();
	void add_exact_string(ANT_string_pair string);
public:
	ANT_pregen_writer_exact_strings(const char * name, int restricted);
	virtual ~ANT_pregen_writer_exact_strings();

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

	int add_field(const char *filename, const char *field_name, pregen_field_type type);

	void process_document(long long doc_index, char *doc_name);
	void process_document(long long doc_index, ANT_string_pair doc_name);

	void close();
};

const char *pregen_type_to_str(pregen_field_type type);

#endif
