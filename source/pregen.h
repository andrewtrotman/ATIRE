#ifndef PREGEN_H_
#define PREGEN_H_

#include <utility>

#include "fundamental_types.h"

#include "file.h"
#include "search_engine_accumulator.h"
#include "string_pair.h"
#include "str.h"
#include "arithmetic_coding.h"
#include "arithmetic_model.h"
#include "arithmetic_model_unigram.h"
#include "arithmetic_model_bigram.h"
#include "unicode.h"
#include "unicode_tables.h"

#define MAX_PREGEN_FIELDS 128
#define PREGEN_FILE_VERSION 2

#define CHAR_ENCODE_FAIL 255

enum pregen_field_type { INTEGER, BINTRUNC, STRTRUNC, BASE32, BASE32_ARITHMETIC, BASE36,
	BASE37, BASE40, RECENTDATE, INTEGEREXACT, STREXACT, STREXACT_RESTRICTED, BASE37_ARITHMETIC,
	ASCII_PRINTABLES, ASCII_PRINTABLES_ARITHMETIC, ASCII_PRINTABLES_ARITHMETIC_BIGRAM};

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

/* Encode a character as a base-37 digit (' ', 0-9, a-z), or CHAR_ENCODE_FAIL if
 * there is no mapping for this character */
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

	virtual void add_field(long long docindex, ANT_string_pair content) = 0;
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
		static unsigned int symbol_frequencies[] =
			{
			1433, 21, 50, 39, 11, 8, 5, 1, 6, 5, 4, 688, 98, 363, 250, 922,
			140, 200, 265, 462, 14, 70, 342, 226, 579, 557, 334, 16, 636, 474, 574, 181,
			74, 184, 23, 117, 20
			};

		arithmetic_model = new ANT_arithmetic_model_unigram(ANT_encode_char_base37::num_symbols, symbol_frequencies, 0);
		}
	else if (type == ASCII_PRINTABLES_ARITHMETIC)
		{
		static unsigned int symbol_frequencies[] =
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

		arithmetic_model = new ANT_arithmetic_model_unigram(ANT_encode_char_printable_ascii::num_symbols, symbol_frequencies, 0);
		}
	else if (type == BASE32_ARITHMETIC)
		{
		static unsigned int symbol_frequencies[] =
			{
			1724, /* Space */
			78, 58, 16, 6, 10, /* Merged digits 0-9 */
			827, 115, 443, 302, 1110, 163, 244, 314, 557, 17,
			85, 411, 274, 702, 670, 411, 18, 760, 567, 689, 213,
			88, 223, 26, 139, 25
			};

		arithmetic_model = new ANT_arithmetic_model_unigram(ANT_encode_char_base32::num_symbols, symbol_frequencies, 0);
		}
	else if (type == ASCII_PRINTABLES_ARITHMETIC_BIGRAM)
		{
		static unsigned int symbol_frequencies[] =
			{
			604, 9, 4, 1, 1, 1, 5, 9, 8, 8, 2, 1, 59, 29, 32, 14,
			22, 39, 9, 27, 6, 7, 23, 15, 12, 37, 78, 5, 1, 1, 1, 8,
			1, 1, 1, 1, 1, 1, 1, 332, 42, 130, 98, 359, 93, 83, 124, 348,
			39, 134, 179, 122, 248, 236, 53, 10, 299, 195, 147, 84, 17, 103, 2, 175,
			158, 1, 1, 1, 2, 6052, 275, 3, 40, 1, 1, 1, 92, 1, 274, 9,
			10, 5, 15, 248, 4, 54, 7, 511, 73, 12, 7, 7, 5, 9, 11, 8,
			3, 1, 1, 1, 1, 14, 2, 6, 1, 1, 1, 2, 1, 692, 425, 657,
			326, 198, 1025, 341, 295, 595, 366, 148, 378, 653, 267, 647, 411, 16, 347, 649,
			382, 69, 102, 309, 1, 50, 2294, 1, 1, 1, 3, 103, 2429, 11950, 16, 1,
			1, 1, 2, 1, 8, 198, 12, 2, 11, 36, 25, 10, 1, 6, 1, 1,
			1, 1, 3, 6, 47, 22, 13, 1, 1, 2, 1, 147, 4, 1, 1, 2,
			1, 1, 1, 3, 1, 5, 1, 1, 2, 3, 13, 12, 3, 1, 8, 2,
			2, 4, 12, 1, 5, 13, 3, 1, 3, 5, 1, 1, 1, 1, 1, 1,
			5, 1, 4460, 2, 29, 1, 1, 1, 3, 3, 11, 49, 4, 1, 217, 21,
			17, 7, 1, 9, 3, 1, 1, 1, 2, 1, 1, 2, 7, 6, 1, 1,
			1, 43, 1, 1, 1, 1, 1, 2, 1, 217, 740, 318, 335, 164, 219, 151,
			207, 58, 466, 95, 286, 367, 137, 156, 354, 5, 229, 475, 394, 31, 80, 448,
			2, 59, 57, 1, 1, 1, 1, 1, 1861, 4, 4, 111, 3, 1, 1, 81,
			5, 4, 2, 1, 7, 5, 9, 5, 42, 1689, 1590, 857, 558, 428, 380, 294,
			371, 193, 4, 1, 1, 1, 1, 21, 2, 1, 1, 1, 1, 1, 3, 12,
			5, 5, 2, 5, 7, 3, 4, 4, 6, 1, 7, 6, 1, 2, 3, 1,
			14, 105, 4, 1, 1, 22, 15, 1, 1, 1, 1, 1, 1, 1, 183, 1,
			1, 4, 271, 4, 1, 1, 2, 3, 1, 1, 2, 2, 20, 2, 1, 159,
			77, 24, 18, 126, 50, 15, 35, 4, 1, 1, 1, 1, 1, 7, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 5,
			2, 6, 1, 2, 1, 1, 2, 9, 3, 5, 1, 2, 1, 1, 1, 1,
			1, 1, 1, 23, 254, 1, 1, 4, 1, 11, 1, 1, 1, 1, 2, 1,
			3, 1, 1, 2, 1, 2, 24, 6, 1, 1, 1, 3, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 2, 1, 4, 1, 2,
			1, 1, 4, 2, 1, 1, 1, 2, 7, 1, 1, 1, 4, 2, 2, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 11114, 1, 1, 1, 1, 1, 3476, 1, 1, 1, 1, 236, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			349, 1, 2, 1, 1, 1, 1, 96, 1, 4, 1, 1, 5, 1, 4, 1,
			3, 1, 1, 3, 3, 3, 2, 3, 2, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 25, 58, 25, 54, 17, 10, 8, 20, 5,
			16, 13, 32, 106, 39, 6, 12, 2, 46, 7396, 85, 2, 12, 23, 1, 7,
			11, 1, 1, 1, 1, 1, 208, 1, 12, 5, 1, 1, 16, 3, 3, 1,
			3, 3, 1, 5, 1, 1, 2, 2611, 68, 19, 11, 5, 10, 6, 11, 10,
			1, 1, 1, 1, 1, 144, 3, 1, 1, 1, 1, 3, 1, 582, 935, 588,
			559, 406, 297, 256, 378, 129, 395, 170, 295, 681, 544, 522, 350, 8, 269, 867,
			239, 92, 133, 1137, 49, 379, 300, 1, 1, 1, 7, 2, 13683, 6, 6, 1,
			1, 1, 39, 19, 40, 7, 4, 1, 958, 350, 86, 95, 1, 25, 1, 1,
			1, 1, 1, 1, 1, 1, 45, 103, 1, 4, 1, 83, 1, 1, 1, 1,
			1, 1, 2, 70, 51, 31, 20, 19, 8, 9, 13, 30, 8, 13, 22, 38,
			22, 30, 12, 1, 25, 71, 27, 3, 23, 108, 1, 32, 25, 1, 1, 1,
			1, 1, 4403, 7, 19, 3, 1, 1, 15, 1, 46, 26, 6600, 1, 68, 35,
			14, 16, 29, 193, 32, 12, 3, 3, 5, 2, 2, 5, 6, 1, 1, 1,
			1, 13, 2, 3, 1, 5, 1, 6, 1, 234, 192, 251, 125, 116, 160, 113,
			198, 119, 109, 57, 254, 283, 303, 162, 241, 4, 217, 534, 203, 116, 88, 233,
			1, 35, 12, 1, 1, 2, 55, 1, 8016, 1, 4, 3, 1, 1, 10, 1,
			2, 334, 1, 326, 122, 83, 20, 314, 2, 74, 11, 15, 1, 4, 8, 3,
			2, 3, 8, 8, 1, 1, 1, 45, 1, 1, 2, 5, 1, 2, 1, 108,
			97, 141, 86, 130, 300, 56, 97, 47, 102, 45, 96, 262, 69, 125, 58, 2,
			117, 275, 48, 28, 24, 110, 1, 12, 6, 1, 1, 1, 1, 1, 10248, 1,
			4, 1, 1, 1, 1, 1, 2, 2, 1, 1, 12, 1, 1, 2, 2, 20,
			4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 27, 47, 35, 16, 18, 15, 31, 17, 21, 15, 18,
			23, 48, 24, 17, 53, 1, 21, 65, 22, 5, 11, 36, 1, 18, 70, 1,
			1, 1, 1, 1, 3417, 1, 2, 1, 1, 1, 19, 1, 3, 3, 1, 1,
			1, 492, 1, 1, 17, 1637, 803, 15, 8, 16, 16, 14, 12, 6, 1, 1,
			1, 1, 1, 18, 4, 1, 1, 1, 1, 1, 1, 83, 117, 100, 81, 38,
			46, 95, 90, 48, 44, 46, 69, 150, 57, 85, 79, 3, 69, 129, 56, 148,
			22, 128, 1, 63, 91, 1, 1, 1, 1, 5, 6072, 3, 10, 1, 1, 1,
			12, 2, 9, 63, 4, 1, 990, 22, 757, 16, 9, 231, 25, 5, 3, 3,
			3, 2, 3, 1, 4, 14, 1, 1, 1, 17, 1, 1, 1, 1, 1, 1,
			1, 72, 29, 85, 24, 20, 15, 14, 24, 15, 56, 22, 21, 27, 21, 20,
			22, 1, 17, 43, 16, 4, 11, 45, 1, 59, 25, 1, 1, 1, 2, 2,
			1851, 1, 3, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 3, 3, 28,
			31, 414, 177, 39, 20, 32, 22, 20, 18, 16, 1, 1, 1, 1, 1, 7,
			1, 1, 1, 1, 1, 1, 1, 300, 442, 478, 319, 221, 218, 286, 346, 146,
			301, 222, 288, 644, 190, 374, 340, 13, 308, 684, 237, 45, 112, 1288, 1, 451,
			619, 1, 1, 1, 1, 2, 1165, 2, 3, 1, 1, 2, 5, 484, 1, 136,
			1, 24, 254, 508, 37, 69, 2815, 380, 286, 306, 338, 322, 271, 271, 278, 234,
			6, 11, 1, 1, 1, 31, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1,
			1, 1, 2, 1, 1, 5, 1, 1, 1, 1, 1, 1, 1, 1, 3, 333,
			21, 1, 1, 1, 1, 4, 1, 1, 1, 1, 2, 6, 227, 1, 1, 1,
			1, 1, 1, 1, 1, 72, 1, 1, 72, 178, 13, 29, 211, 127, 133, 101,
			96, 124, 169, 613, 2907, 3632, 9, 3, 1, 1, 1, 4, 1, 1, 1, 1,
			1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 12, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 3, 1278, 1, 4, 1, 1, 1, 3, 1, 1, 300, 1, 3, 356, 801,
			62, 147, 5169, 506, 471, 494, 481, 568, 469, 484, 530, 491, 6, 12, 1, 1,
			1, 32, 2, 1, 1, 3, 1, 5, 1, 1, 1, 3, 2, 1, 1, 1,
			1, 5, 1, 1, 2, 4, 53, 1, 1, 1, 1, 1, 6, 1, 1, 1,
			3, 2, 1, 1, 1, 1, 3, 2, 2586, 1, 3, 1, 1, 1, 10, 1,
			1, 639, 1, 7, 822, 1715, 105, 258, 2485, 838, 749, 708, 724, 963, 858, 793,
			913, 691, 14, 19, 1, 1, 1, 41, 2, 1, 1, 9, 1, 8, 2, 2,
			1, 2, 6, 1, 2, 2, 1, 8, 3, 2, 1, 4, 1, 4, 4, 1,
			117, 2, 18, 1, 1, 1, 6, 6, 1, 1, 1, 2, 5, 10, 2496, 2,
			5, 1, 1, 1, 21, 1, 7, 673, 1, 7, 783, 1626, 127, 283, 1544, 844,
			650, 661, 748, 809, 667, 662, 745, 709, 10, 25, 1, 1, 1, 48, 2, 1,
			1, 4, 1, 4, 1, 2, 1, 1, 3, 1, 1, 1, 1, 9, 4, 2,
			1, 4, 5, 4, 2, 1, 2, 1, 101, 1, 4, 1, 1, 5, 1, 1,
			1, 2, 8, 1, 2018, 1, 9, 1, 1, 1, 16, 1, 3, 514, 1, 8,
			608, 1343, 113, 183, 1780, 638, 491, 469, 435, 532, 472, 479, 480, 490, 12, 23,
			1, 2, 1, 39, 3, 1, 1, 5, 1, 3, 1, 1, 1, 2, 1, 1,
			1, 1, 2, 10, 1, 1, 1, 4, 2, 8, 1, 1, 1, 3, 70, 1,
			4, 1, 2, 4, 1, 1, 1, 1, 2, 2, 1503, 1, 2, 1, 1, 1,
			26, 1, 1, 368, 1, 3, 476, 1082, 77, 151, 1504, 530, 445, 525, 464, 569,
			483, 478, 512, 532, 6, 15, 1, 1, 1, 29, 1, 1, 1, 9, 1, 2,
			1, 1, 1, 2, 1, 1, 1, 1, 1, 5, 1, 1, 2, 6, 3, 1,
			1, 1, 1, 1, 52, 1, 1, 2, 1, 3, 1, 1, 1, 1, 2, 2,
			1353, 1, 3, 1, 1, 1, 7, 1, 2, 295, 2, 3, 423, 995, 51, 144,
			1587, 628, 553, 607, 707, 837, 878, 956, 1150, 1404, 7, 13, 1, 2, 1, 17,
			1, 1, 1, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 7,
			3, 1, 1, 2, 4, 4, 1, 1, 1, 1, 99, 1, 1, 1, 2, 3,
			1, 1, 1, 1, 5, 1, 454, 1, 1, 1, 1, 1, 1, 1, 1, 111,
			1, 2, 139, 287, 20, 31, 866, 531, 537, 669, 611, 847, 807, 811, 943, 839,
			2, 5, 1, 1, 1, 11, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 2, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1,
			20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 498, 1, 1, 1,
			1, 1, 1, 1, 1, 145, 2, 1, 184, 415, 27, 45, 1740, 1699, 1550, 1289,
			992, 807, 801, 741, 709, 1110, 2, 2, 1, 1, 1, 7, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 25, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 14702, 1, 2, 1, 1, 1, 1, 1, 1, 5, 1, 2, 1, 1,
			1, 1, 1, 15, 3, 1, 1, 1, 1, 1, 1, 1, 8, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 4, 4, 1, 2, 5, 2,
			6, 1, 5, 1, 3, 4, 4, 1, 5, 1, 3, 7, 1, 3, 1, 4,
			1, 7, 22, 1, 1, 1, 1, 1, 11447, 1, 1, 1, 1, 1, 72, 1,
			2, 3, 1, 1, 9, 6, 2, 12, 1, 115, 4, 1, 1, 1, 1, 1,
			1, 1, 1, 4, 1, 2, 1, 18, 1, 1, 1, 1, 1, 1, 1, 121,
			52, 125, 25, 30, 38, 70, 41, 199, 45, 96, 61, 239, 163, 142, 97, 6,
			25, 108, 159, 68, 47, 191, 1, 8, 5, 1, 1, 1, 1, 3, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 5605, 2, 4, 1, 1, 2, 630, 4, 20, 153, 1, 4,
			3, 34, 4, 16, 14, 334, 66, 13, 9, 9, 4, 18, 11, 13, 14, 1,
			1, 980, 1, 57, 1, 10, 2, 7, 3, 4, 1, 240, 312, 361, 157, 200,
			138, 232, 191, 146, 217, 94, 215, 489, 188, 124, 198, 5, 181, 293, 159, 127,
			98, 197, 1, 44, 20, 5, 1, 18, 1, 6, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			2492, 75, 19, 1, 1, 1, 62, 3, 14, 1680, 1, 1, 243, 194, 47, 66,
			1, 36, 1, 1, 1, 1, 1, 1, 1, 1, 22, 9, 1, 1, 1, 5255,
			1, 1, 1, 38, 1, 15, 2, 13, 21, 24, 18, 11, 11, 12, 15, 10,
			8, 10, 13, 24, 10, 13, 20, 1, 18, 38, 21, 5, 10, 39, 1, 4,
			2, 1, 1, 8, 1, 3, 5067, 6, 1, 7, 1, 1, 1, 1, 1, 9,
			1, 1, 1, 5, 5, 2, 1, 2344, 14, 11, 13, 2, 4, 10, 4, 6,
			1, 1, 1, 1, 1, 2, 166, 1, 1, 1, 1, 3, 1, 710, 123, 278,
			298, 155, 80, 77, 270, 81, 97, 165, 57, 257, 84, 51, 124, 14, 156, 185,
			81, 53, 32, 114, 2, 266, 10, 1, 1, 1, 1, 1, 536, 1, 15, 6,
			1, 1, 11, 1, 1, 1, 4, 14, 17, 45, 4, 1, 15, 3198, 58, 20,
			21, 7, 6, 9, 5, 7, 1, 1, 1, 1, 1, 128, 1, 10, 1, 1,
			1, 1, 1, 607, 1027, 634, 421, 533, 279, 345, 434, 130, 285, 242, 455, 911,
			522, 851, 516, 15, 368, 1038, 367, 130, 120, 536, 1, 39, 33, 1, 1, 1,
			1, 8, 1015, 1, 3, 1, 1, 1, 1, 1, 3, 2, 2, 1, 7, 5,
			5, 19, 5, 104, 31, 5, 7, 5, 8, 3, 10, 5, 2, 1, 1, 1,
			1, 4, 2, 1, 1279, 1, 1, 2, 1, 163, 399, 442, 247, 259, 334, 270,
			299, 136, 155, 137, 325, 578, 182, 165, 268, 10, 270, 644, 311, 56, 87, 335,
			1, 32, 20, 1, 2, 1, 1, 2, 12239, 3, 26, 2, 1, 1, 21, 9,
			63, 58, 3, 10, 977, 214, 726, 94, 1, 19, 3, 1, 1, 1, 1, 2,
			2, 1, 69, 65, 1, 10, 1, 32, 1, 42, 5, 35, 1, 1, 7, 66,
			62, 83, 41, 43, 32, 41, 39, 18, 25, 32, 66, 75, 24, 33, 49, 1,
			34, 146, 19, 5, 34, 62, 1, 20, 1, 6, 1, 2, 1, 5, 80, 1,
			1, 1, 1, 1, 8, 1, 1, 11, 4, 1, 37, 5, 1, 2, 7, 5,
			4, 4, 2, 7, 4, 1, 1, 1, 4, 2, 1, 3, 1, 2, 1, 1,
			1, 1, 47, 25, 1, 83, 1, 18, 6, 22, 1, 2, 6, 71, 14, 44,
			4, 3, 6, 9, 5, 1, 5, 7, 28, 13, 1, 3, 1, 3, 1, 1,
			1, 1, 17, 1, 903, 1, 1, 1, 1, 1, 2, 2, 4, 53, 1, 1,
			29, 39, 211, 4, 4, 1745, 36, 10, 1, 3, 1, 1, 2, 2, 1, 2,
			1, 1, 1, 110, 2, 1, 1, 1, 5, 1319, 1, 934, 244, 277, 205, 289,
			152, 152, 222, 78, 470, 85, 370, 462, 101, 88, 123, 3, 300, 221, 139, 28,
			87, 240, 1, 6, 10, 1, 1, 1, 1, 1, 235, 2, 1, 1, 1, 1,
			2, 1, 1, 2, 2, 1, 18, 3, 5, 8, 3, 79, 1, 4, 6, 1,
			1, 2, 16, 23, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1,
			273, 81, 134, 67, 85, 37, 30, 31, 47, 12, 18, 6, 55, 214, 65, 73,
			36, 19, 149, 4981, 88, 21, 23, 20, 1, 2, 3, 1, 1, 1, 4, 1,
			1292, 3, 5, 1, 1, 1, 14, 39, 2, 37, 1, 1, 260, 32, 117, 51,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 2, 1, 1, 1, 13,
			1, 1, 1, 1, 1, 3, 1, 49, 237, 374, 279, 85, 48, 299, 140, 165,
			15, 161, 1067, 1822, 3400, 8, 148, 8, 2176, 516, 833, 364, 162, 119, 16, 169,
			61, 1, 1, 1, 1, 6, 470, 2, 4, 1, 1, 1, 1, 3, 2, 6,
			1, 1, 22, 10, 478, 14, 1, 15, 1, 1, 1, 1, 1, 1, 1, 1,
			19, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1236, 81, 13,
			5, 2691, 1, 3, 3, 2083, 6, 31, 567, 9, 26, 1155, 4, 1, 1053, 99,
			119, 627, 1, 1, 1, 200, 2, 1, 1, 1, 2, 6, 297, 2, 2, 1,
			1, 1, 3, 8, 5, 10, 1, 1, 49, 3, 208, 20, 1, 15, 1, 1,
			1, 1, 1, 1, 1, 1, 13, 3, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1776, 4, 266, 11, 1788, 7, 13, 3912, 709, 1, 1764, 328, 10,
			15, 2780, 12, 5, 218, 50, 443, 172, 2, 3, 1, 107, 226, 1, 1, 1,
			1, 2, 4533, 18, 9, 1, 1, 1, 9, 37, 3, 47, 1, 1, 271, 40,
			438, 88, 1, 7, 1, 1, 1, 1, 1, 1, 1, 1, 22, 4, 1, 1,
			1, 20, 1, 1, 1, 2, 1, 4, 1, 1988, 20, 14, 184, 2664, 137, 113,
			27, 1191, 10, 14, 178, 102, 116, 940, 71, 2, 615, 393, 156, 180, 21, 250,
			1, 150, 120, 1, 1, 1, 1, 5, 849, 2, 9, 1, 1, 1, 1, 8,
			2, 16, 1, 1, 101, 13, 34, 17, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1405, 3, 1, 1, 1, 15, 1, 1, 1, 1, 1, 1, 1, 278,
			78, 137, 255, 160, 29, 68, 47, 135, 4, 97, 703, 183, 623, 78, 106, 14,
			1632, 587, 257, 27, 62, 106, 36, 74, 15, 1, 1, 1, 1, 2, 1559, 1,
			2, 1, 1, 1, 1, 10, 1, 6, 1, 1, 49, 6, 104, 14, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 2419, 1, 12, 2, 415, 312, 3, 5, 318, 1, 6,
			145, 13, 4, 1592, 1, 1, 1268, 17, 46, 99, 1, 1, 1, 6, 1, 1,
			1, 1, 1, 1, 2363, 2, 5, 1, 1, 1, 4, 40, 8, 42, 1, 1,
			387, 61, 87, 108, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 12, 5,
			1, 1, 1, 18, 1, 1, 1, 1, 1, 1, 1, 890, 130, 6, 39, 2765,
			15, 183, 423, 416, 2, 18, 991, 97, 118, 472, 1, 7, 871, 279, 331, 308,
			2, 25, 1, 181, 3, 1, 1, 1, 1, 14, 1477, 1, 7, 1, 1, 1,
			5, 17, 1, 25, 1, 1, 229, 21, 108, 47, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 7, 3, 1, 1, 1, 6, 1, 1, 1, 1, 1, 1,
			1, 1616, 15, 5, 6, 2368, 7, 3, 2, 1404, 1, 39, 294, 95, 764, 928,
			5, 1, 248, 51, 190, 385, 6, 34, 1, 83, 3, 1, 1, 1, 1, 3,
			366, 4, 1, 1, 1, 1, 1, 31, 2, 18, 1, 1, 171, 19, 19, 44,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 1, 1, 1, 11,
			1, 1, 1, 1, 1, 1, 1, 899, 66, 1209, 262, 1168, 127, 398, 10, 20,
			33, 104, 1832, 737, 3168, 572, 199, 2, 414, 918, 1004, 48, 221, 10, 13, 5,
			110, 1, 1, 1, 1, 5, 249, 1, 2, 1, 1, 1, 6, 1, 1, 11,
			1, 1, 19, 2, 785, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			2, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 2, 1, 4095, 1, 54,
			39, 1371, 2, 1, 4, 170, 2, 25, 12, 1, 39, 5767, 68, 1, 372, 49,
			25, 987, 1, 4, 1, 2, 3, 1, 1, 1, 1, 11, 1423, 3, 15, 1,
			1, 1, 2, 43, 8, 60, 1, 2, 467, 109, 57, 99, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 5, 5, 1, 1, 1, 10, 1, 1, 1, 2,
			1, 1, 1, 905, 7, 13, 4, 1583, 13, 13, 35, 2469, 1, 7, 372, 63,
			111, 688, 6, 1, 155, 389, 16, 379, 7, 20, 1, 366, 1, 1, 1, 1,
			1, 11, 604, 1, 7, 1, 1, 1, 2, 11, 2, 12, 1, 1, 102, 10,
			65, 15, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1,
			1, 3, 1, 1, 1, 1, 1, 1, 1, 940, 91, 36, 228, 1304, 37, 10,
			39, 1205, 4, 172, 1174, 168, 77, 816, 102, 1, 7, 150, 248, 122, 63, 28,
			1, 930, 19, 1, 1, 1, 1, 3, 861, 1, 4, 1, 1, 1, 1, 2,
			1, 6, 1, 1, 38, 4, 181, 5, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 2, 2, 1, 1, 1, 6, 1, 1, 1, 1, 1, 1, 1, 3115,
			298, 69, 52, 2230, 4, 2, 8, 2172, 1, 22, 29, 1130, 41, 449, 410, 1,
			107, 107, 19, 197, 1, 272, 2, 172, 4, 1, 1, 1, 1, 4, 3200, 5,
			8, 1, 1, 1, 13, 49, 12, 68, 1, 2, 515, 62, 53, 100, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 10, 10, 1, 1, 1, 25, 1, 1,
			1, 3, 1, 2, 2, 1058, 60, 463, 1651, 1524, 279, 1154, 35, 793, 55, 472,
			37, 9, 829, 411, 4, 10, 153, 763, 888, 44, 18, 26, 7, 392, 62, 1,
			1, 1, 1, 2, 567, 2, 1, 1, 1, 1, 3, 17, 3, 13, 1, 1,
			126, 14, 143, 13, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 5,
			1, 1, 1, 8, 1, 1, 1, 1, 1, 1, 1, 60, 625, 127, 121, 155,
			588, 139, 461, 72, 18, 526, 867, 563, 1345, 614, 136, 1, 1900, 445, 348, 641,
			145, 440, 14, 48, 27, 1, 1, 1, 1, 2, 721, 35, 6, 1, 1, 1,
			2, 7, 1, 12, 1, 1, 75, 27, 170, 18, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 18, 1094, 1, 1, 1, 33, 1, 1, 1, 1, 1, 1,
			1, 1964, 24, 15, 61, 1255, 168, 25, 1384, 506, 1, 78, 478, 14, 19, 868,
			430, 2, 548, 215, 219, 118, 10, 2, 1, 24, 3, 1, 1, 1, 1, 1,
			68, 1, 1, 1, 1, 1, 3, 3, 1, 2, 1, 1, 43, 4, 76, 2,
			1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 1, 1, 1, 2,
			1, 1, 1, 1, 1, 1, 1, 8, 1, 118, 3, 9, 1, 1, 3, 4,
			1, 5, 21, 4, 3, 4, 1, 11, 3, 1, 7, 8057, 11, 5, 1, 1,
			1, 1, 1, 1, 1, 6, 1482, 2, 5, 2, 1, 1, 2, 30, 3, 24,
			1, 1, 256, 39, 102, 62, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			4, 2, 1, 1, 1, 4, 1, 1, 1, 1, 1, 2, 1, 928, 158, 322,
			289, 4161, 65, 311, 40, 1054, 5, 135, 311, 751, 664, 947, 26, 3, 268, 480,
			485, 247, 47, 18, 1, 652, 105, 1, 1, 1, 1, 9, 3191, 25, 6, 1,
			1, 2, 4, 16, 9, 85, 1, 1, 363, 73, 248, 122, 1, 3, 1, 1,
			1, 1, 1, 1, 1, 1, 44, 5, 1, 1, 1, 44, 1, 1, 1, 2,
			1, 1, 1, 779, 105, 744, 65, 1617, 11, 18, 464, 748, 2, 1215, 235, 129,
			48, 726, 206, 10, 63, 709, 2155, 628, 37, 128, 1, 103, 98, 1, 1, 1,
			1, 7, 1079, 3, 6, 1, 1, 1, 3, 21, 4, 22, 1, 1, 124, 25,
			165, 36, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 109, 110, 1, 1,
			1, 18, 1, 1, 1, 2, 1, 2, 1, 532, 5, 57, 1, 1228, 14, 17,
			1319, 918, 7, 37, 142, 43, 58, 1081, 3, 1, 421, 340, 491, 299, 4, 43,
			34, 339, 218, 1, 1, 1, 1, 4, 87, 4, 1, 1, 1, 1, 1, 1,
			1, 4, 1, 1, 13, 4, 17, 2, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 2, 1, 1, 1, 6, 1, 1, 1, 1, 1, 1, 1, 407,
			280, 367, 188, 449, 128, 329, 42, 265, 24, 161, 480, 400, 2060, 6, 357, 4,
			1197, 1048, 398, 2, 35, 14, 18, 34, 66, 1, 1, 1, 1, 2, 224, 1,
			2, 1, 1, 1, 2, 1, 2, 18, 1, 1, 41, 6, 326, 8, 1, 2,
			1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 2, 1, 1,
			1, 1, 1, 1, 1, 2661, 1, 15, 2, 4191, 1, 4, 2, 4024, 1, 25,
			34, 1, 34, 499, 1, 1, 39, 72, 95, 27, 2, 1, 1, 66, 1, 1,
			1, 1, 1, 12, 1076, 5, 2, 1, 1, 1, 1, 9, 2, 13, 1, 1,
			90, 11, 286, 46, 1, 5, 4, 1, 1, 1, 1, 1, 1, 1, 6, 2,
			1, 1, 1, 12, 2, 1, 1, 1, 1, 2, 1, 2204, 27, 12, 21, 1381,
			62, 5, 445, 3500, 6, 36, 79, 75, 415, 535, 104, 1, 475, 836, 36, 164,
			64, 67, 1, 952, 4, 1, 1, 1, 1, 9, 4320, 3, 21, 1, 1, 1,
			41, 36, 12, 172, 3, 3, 1014, 287, 497, 253, 1, 7, 3, 1, 1, 1,
			1, 1, 2, 1, 20, 9, 1, 1, 1, 61, 1, 1, 1, 6, 1, 4,
			1, 4680, 91, 99, 6, 554, 298, 11, 54, 881, 1, 6, 189, 23, 21, 635,
			128, 1, 12, 345, 743, 36, 60, 133, 532, 14, 2, 1, 1, 1, 1, 2,
			5943, 11, 50, 1, 1, 1, 27, 53, 31, 123, 1, 1, 945, 135, 120, 153,
			1, 3, 1, 1, 1, 2, 1, 1, 1, 1, 46, 10, 1, 1, 1, 65,
			3, 1, 1, 2, 1, 3, 1, 246, 67, 141, 204, 401, 6, 89, 11, 74,
			6, 78, 453, 96, 350, 3352, 19, 1, 102, 196, 95, 506, 28, 130, 1, 2,
			31, 1, 1, 3, 3, 4, 154, 1, 2, 1, 1, 1, 1, 2, 3, 9,
			1, 1, 62, 6, 14, 16, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			2, 2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 955, 22, 44,
			35, 1077, 2, 19, 15, 2912, 3, 39, 53, 84, 45, 1178, 2, 2, 22, 42,
			1, 1388, 22, 615, 1, 603, 41, 1, 1, 1, 1, 19, 358, 1, 9, 1,
			1, 1, 2, 1, 1, 5, 2, 1, 6, 24, 3, 1, 6, 169, 17, 5,
			2, 2, 3, 5, 2, 2, 6, 1, 1, 3, 1, 34, 1, 1, 1, 1,
			1, 1, 1, 241, 417, 326, 206, 166, 139, 233, 241, 103, 152, 130, 224, 563,
			218, 127, 299, 19, 165, 393, 154, 31, 41, 219, 3, 24, 44, 26, 1, 2,
			1, 1, 382, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1,
			1, 1, 1, 1, 2, 1, 1, 1, 1, 9, 14, 10, 21, 13, 17, 17,
			9, 1, 3, 2, 2, 14, 2, 12, 7, 1, 7, 12, 11, 3, 10, 12,
			1, 3, 1, 1, 31, 1, 1, 6, 2121, 3, 2, 1, 1, 1, 8, 18,
			9, 21, 1, 1, 81, 19, 28, 13, 1, 16, 1, 1, 1, 1, 1, 2,
			1, 2, 4, 5, 1, 4, 1, 13, 1, 1, 1, 3, 1, 1, 1, 13,
			24, 23, 22, 4, 10, 13, 19, 11, 5, 12, 9, 27, 9, 14, 4, 1,
			14, 23, 11, 1, 8, 9, 1, 1, 1, 22, 1, 35, 2, 1, 5041, 13,
			1, 1, 1, 1, 14, 2, 2, 16, 28, 1, 9, 2, 1, 1, 3, 2104,
			6, 1, 2, 2, 7, 6, 1, 5, 1, 1, 1, 1, 1, 4, 1, 1,
			1, 1, 4, 1, 3, 81, 71, 135, 74, 31, 42, 54, 54, 29, 40, 39,
			82, 106, 68, 110, 103, 4, 77, 108, 56, 26, 18, 76, 1, 7, 2, 1,
			1, 1, 579, 3, 539, 2, 1, 1, 1, 1, 2, 1, 2, 5, 1, 1,
			27, 6, 4, 5, 1, 56, 7, 1, 1, 2, 1, 1, 3, 10, 26, 1,
			1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 62, 86, 47, 56, 62,
			26, 43, 45, 19, 26, 25, 117, 56, 188, 51, 37, 6, 176, 165, 73, 9,
			14, 21, 8, 6, 11, 1, 1, 1, 2, 8984
			};

		arithmetic_model = new ANT_arithmetic_model_bigram(ANT_encode_char_printable_ascii::num_symbols, symbol_frequencies, 0);
		}
	else
		arithmetic_model = NULL;
	}

	virtual void add_field(long long docindex, ANT_string_pair content);

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

	virtual void add_field(long long docindex, ANT_string_pair content);

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

	virtual void add_field(long long docindex, ANT_string_pair content);

	virtual int open_write(const char * filename);
	virtual void close_write();
};

/**
 * Generates pregens for a number of different fields on a document collection simultaneously.
 */
class ANT_pregens_writer
{
private:
	ANT_string_pair strip_garbage(const ANT_string_pair & s);

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
