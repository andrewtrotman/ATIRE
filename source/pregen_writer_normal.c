/*
	PREGEN_WRITER_NORMAL.C
	----------------------
*/
#include <assert.h>
#include <limits>
#include "maths.h"
#include "pregen_writer_normal.h"
#include "encode_char_8bit.h"
#include "encode_char_base36.h"
#include "encode_char_base40.h"
#include "unicode.h"
#include "arithmetic_coding_encoder.h"

/*
	ANT_PREGEN_WRITER_NORMAL::GENERATE_INTEGER()
	--------------------------------------------
*/
ANT_pregen_t ANT_pregen_writer_normal::generate_integer(ANT_string_pair field)
{
long value;

/* 
	atol can happily wander off the end of the field. It'll be stopped by the null-termination of the document name and not cause a problem.
*/
value = atol(field.start);

return value > std::numeric_limits<ANT_pregen_t>::max() ? std::numeric_limits<ANT_pregen_t>::max() : value <= 0 ? 1 : (ANT_pregen_t) value;
}

/*
	ANT_PREGEN_WRITER_NORMAL::GENERATE_BINTRUNC()
	---------------------------------------------
*/
ANT_pregen_t ANT_pregen_writer_normal::generate_bintrunc(ANT_string_pair field)
{
ANT_pregen_t result = 0;
size_t bytes = ANT_min(field.length(), sizeof(result));

/* 
	Work correctly on both little and big-endian systems (don't just cast string to integer) 
*/
for (unsigned int i = 0; i < bytes; i++)
	result = (result << CHAR_BIT) | field[i];

/* 
	Left-align the result 
*/
if (bytes < sizeof(result))
	result = result << (CHAR_BIT * (sizeof(result) - bytes));

return result;
}

/*
	ANT_PREGEN_WRITER_NORMAL::GENERATE_RECENTDATE()
	-----------------------------------------------
	For severely constrained accumulator sizes, compress UNIX timestamps to give higher accuracy
	for recent dates than old ones.
*/
ANT_pregen_t ANT_pregen_writer_normal::generate_recentdate(ANT_string_pair field)
{
long long date = atol(field.start);

//Don't need to do any fancy conversion
#pragma ANT_PRAGMA_CONST_CONDITIONAL
if (sizeof(ANT_pregen_t) >= 4)
	return (ANT_pregen_t) date;

return 0; //TODO implement me
}

/*
	ANT_PREGEN_WRITER_NORMAL::GENERATE_SLIDING_RADIX()
	--------------------------------------------------
*/
template <typename T> 
ANT_pregen_t ANT_pregen_writer_normal::generate_sliding_radix(ANT_string_pair field)
{
return 0;
}

/*
	ANT_PREGEN_WRITER_NORMAL::GENERATE_RADIX()
	------------------------------------------
	Generalized encoding of a UTF-8 string using a character encoding function
	provided as type parameter T, e.g. ANT_encode_char_base36. The string is
	first Unicode-normalized and lowercased, then encoded using the given function.

	T must have a static method num_symbols, and a method encode(unsigned char), which
	takes a character to encode and returns a value in the range [0..num_symbols).
*/
template <typename T>
ANT_pregen_t ANT_pregen_writer_normal::generate_radix(ANT_string_pair field)
{
ANT_pregen_t result = 0;

uint32_t character;
int prev_char_was_space;
unsigned char buffer[UTF8_LONGEST_DECOMPOSITION_LEN];
unsigned char *buffer_pos;
unsigned char encoded_space = T::encode(' ');
size_t buffer_remain;

int dest_chars_remain = ANT_compiletime_int_floor_log_to_base<ANT_pregen_t, T::num_symbols>::value;

const int final_digit_radix = ANT_compiletime_int_floor_log_to_base_remainder<ANT_pregen_t, T::num_symbols, ANT_compiletime_int_floor_log_to_base_has_remainder<ANT_pregen_t, T::num_symbols>::value>::value;

prev_char_was_space = 1; 		// Leading spaces are not significant

while (field.string_length > 0 && (character = utf8_to_wide(field.start)) != 0 && dest_chars_remain >= 0)
	{
	buffer_pos = buffer;
	buffer_remain = sizeof(buffer);

	ANT_UNICODE_normalize_lowercase_toutf8(&buffer_pos, &buffer_remain, character);

	/*
		Write as much of that UTF-8 normalization as we can fit into the result
	*/
	for (int i = 0; i < (buffer_pos - buffer) && dest_chars_remain >= 0; i++)
		{
		unsigned char encoded = T::encode(buffer[i]);

		if (encoded != CHAR_ENCODE_FAIL)
			{
			if (encoded == encoded_space)
				if (prev_char_was_space)
					continue; //Strip multiple spaces in a row
				else
					prev_char_was_space = 1;
			else
				prev_char_was_space = 0;


			if (dest_chars_remain == 0)
				{
				/*
					Final digit isn't the full radix, scale down
				*/
				result = (ANT_pregen_t) (result * final_digit_radix + (encoded * (final_digit_radix - 1)) / (T::num_symbols - 1));
				}
			else
				result = (ANT_pregen_t) (result * T::num_symbols + encoded);

			dest_chars_remain--;
			}
		}

	field.string_length -= utf8_bytes(field.start);
	field.start += utf8_bytes(field.start);
	}

/*
	"left justify" the resulting bits so that longer strings aren't always larger than shorter ones
*/
if (dest_chars_remain >= 0)
	{
	while (dest_chars_remain > 0)
		{
		result = (ANT_pregen_t) (result * T::num_symbols);
		dest_chars_remain--;
		}
	/*
		Final digit isn't the full radix, scale down
	*/
	result = (ANT_pregen_t) (result * final_digit_radix);
	}

if (result == 0) // Strings of all zero symbols become zero, which we don't want as an RSV. Avoid.
	return 1;

return result;
}

/*
	ANT_PREGEN_WRITER_NORMAL::GENERATE_ARITHMETIC()
	-----------------------------------------------
*/
template <typename T>
ANT_pregen_t ANT_pregen_writer_normal::generate_arithmetic(ANT_string_pair field, ANT_arithmetic_model *model)
{
uint32_t character;

unsigned char buffer[UTF8_LONGEST_DECOMPOSITION_LEN];
unsigned char *buffer_pos;
unsigned char encoded_space = T::encode(' ');
int prev_char_was_space;
size_t buffer_remain;
ANT_pregen_t result;

ANT_arithmetic_coding_encoder<ANT_pregen_t> encoder(model);

prev_char_was_space = 1; //Leading whitespace is not significant

while (field.string_length > 0 && (character = utf8_to_wide(field.start)) != 0)
	{
	buffer_pos = buffer;
	buffer_remain = sizeof(buffer);

	ANT_UNICODE_normalize_lowercase_toutf8(&buffer_pos, &buffer_remain, character);

	/* 
		Write as much of that UTF-8 normalization as we can fit into the result 
	*/
	for (int i = 0; i < (buffer_pos - buffer); i++)
		{
		unsigned char symbol = T::encode(buffer[i]);

		if (symbol != CHAR_ENCODE_FAIL)
			{
			if (symbol == encoded_space)
				if (prev_char_was_space)
					continue; // Strip multiple spaces in a row
				else
					prev_char_was_space = 1;
			else
				prev_char_was_space = 0;

			if (!encoder.encode_symbol(symbol))
				goto break_outer; // Ran out of room in the encoded result, so stop encoding
			}
		}
	field.string_length -= utf8_bytes(field.start);
	field.start += utf8_bytes(field.start);
	}

break_outer:
result = encoder.done();

if (result == 0) // Strings of all zero symbols become zero, which we don't want as an RSV. Avoid.
	return 1;

return result;
}

/*
	ANT_PREGEN_WRITER_NORMAL::GENERATE()
	------------------------------------
*/
ANT_pregen_t ANT_pregen_writer_normal::generate(ANT_string_pair field)
{
switch (type)
	{
	case INTEGER:
		return generate_integer(field);
	case STRTRUNC:
		return generate_radix<ANT_encode_char_8bit>(field);
	case BINTRUNC:
		return generate_bintrunc(field);
	case BASE32:
		return generate_radix<ANT_encode_char_base32>(field);
	case BASE36:
		return generate_radix<ANT_encode_char_base36>(field);
	case BASE37:
		return generate_radix<ANT_encode_char_base37>(field);
	case BASE40:
		return generate_radix<ANT_encode_char_base40>(field);
	case ASCII_PRINTABLES:
		return generate_radix<ANT_encode_char_printable_ascii>(field);
	case BASE32_ARITHMETIC:
		return generate_arithmetic<ANT_encode_char_base32>(field, arithmetic_model);
	case BASE37_ARITHMETIC:
		return generate_arithmetic<ANT_encode_char_base37>(field, arithmetic_model);
	case ASCII_PRINTABLES_ARITHMETIC:
	case ASCII_PRINTABLES_ARITHMETIC_BIGRAM:
		return generate_arithmetic<ANT_encode_char_printable_ascii>(field, arithmetic_model);
	default:
		assert(0);
		return 0;
	}
}

/*
	ANT_PREGEN_WRITER_NORMAL::ADD_FIELD()
	-------------------------------------
*/
void ANT_pregen_writer_normal::add_field(long long docindex, ANT_string_pair content)
{
long long skip_docs;

/* 
	It's possible that previous documents didn't contain this field, so be prepared to zero-pad.
*/
skip_docs = docindex - doc_count;

if (skip_docs > 0)
	add_score(1, skip_docs);
else
	assert(skip_docs == 0); // docindex should never go backwards. We don't support that.

add_score(generate(content));
}

/*
	ANT_PREGEN_WRITER_NORMAL::ADD_SCORE()
	-------------------------------------
*/
void ANT_pregen_writer_normal::add_score(ANT_pregen_t score, long long count)
{
for (int i = 0; i < count; i++)
	file.write((unsigned char *)&score, sizeof(score));
doc_count = (uint32_t)(doc_count + count);
}
