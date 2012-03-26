/*
	PREGEN_WRITER_EXACT_STRINGS.C
	-----------------------------
*/
#include <assert.h>
#include <algorithm>
#include "maths.h"
#include "pregen_t.h"
#include "pregen_writer_exact_strings.h"
#include "encode_char_base37.h"
#include "encode_char_printable_ascii.h"
#include "unicode.h"

/*
	ANT_PREGEN_WRITER_EXACT_STRINGS::ANT_PREGEN_WRITER_EXACT_STRINGS()
	------------------------------------------------------------------
*/
ANT_pregen_writer_exact_strings::ANT_pregen_writer_exact_strings(const char *name, int restricted) : ANT_pregen_writer(restricted ? STREXACT_RESTRICTED : STREXACT, name), memory(100*1024*1024)
{
this->restricted = restricted;
doc_capacity = 1024;
exact_strings = new std::pair<long long, ANT_string_pair>[doc_capacity];
}

/*
	ANT_PREGEN_WRITER_EXACT_STRINGS::~ANT_PREGEN_WRITER_EXACT_STRINGS()
	-------------------------------------------------------------------
*/
ANT_pregen_writer_exact_strings::~ANT_pregen_writer_exact_strings()
{
/*
	The strings themselves are allocated with our memory pool, so they will all be
	destroyed automatically.
*/
delete [] exact_strings;
}

/*
	EXACT_STR_LESS()
	----------------
*/
static bool exact_str_less(const std::pair<long long, ANT_string_pair>& a, const std::pair<long long, ANT_string_pair>& b)
{
//Perform an ordering the same as strcmp (NOT what ANT_string_pair's compare performs)
int result = memcmp(a.second.start, b.second.start, ANT_min(a.second.string_length, b.second.string_length));

if (result < 0)
	return true;

if (result > 0)
	return false;

return a.second.string_length < b.second.string_length;
}

/*
	ANT_PREGEN_WRITER_EXACT_STRINGS::ADD_EXACT_STRING()
	---------------------------------------------------
*/
void ANT_pregen_writer_exact_strings::add_exact_string(ANT_string_pair str)
{
ensure_storage();

exact_strings[doc_count].first = doc_count;
exact_strings[doc_count].second = str;
doc_count++;
}

/*
	ANT_PREGEN_WRITER_EXACT_STRINGS::ENSURE_STORAGE()
	-------------------------------------------------
*/
void ANT_pregen_writer_exact_strings::ensure_storage()
{
if (doc_count >= doc_capacity)
	{
	doc_capacity = doc_capacity * 2 + 1;

	std::pair<long long, ANT_string_pair>* new_strings = new std::pair<long long, ANT_string_pair>[doc_capacity];
	memcpy(new_strings, exact_strings, sizeof(exact_strings[0]) * doc_count);

	delete [] exact_strings;
	exact_strings = new_strings;
	}
}


/*
	ANT_PREGEN_WRITER_EXACT_STRINGS::ADD_FIELD()
	--------------------------------------------
*/
void ANT_pregen_writer_exact_strings::add_field(long long docindex, ANT_string_pair content)
{
long long skip_docs;

ANT_string_pair result;
unsigned char char_decomposition_buffer[UTF8_LONGEST_DECOMPOSITION_LEN];
unsigned char *encoded_string_buffer, *encoded_string_pos;
const unsigned char encoded_space = restricted ? ANT_encode_char_base37::encode(' ') : ANT_encode_char_printable_ascii::encode(' ');
uint32_t character;
int prev_char_was_space = 0;

/*
	It's possible that previous documents didn't contain this field, so be prepared to zero-pad.
*/
skip_docs = docindex - doc_count;

assert(skip_docs >= 0);

while (skip_docs > 0)
	{
	result.start = NULL;
	result.string_length = 0;

	add_exact_string(result);
	skip_docs--;
	}

/* 
	Allocate enough room for worst-case result (actually pretty damn pessimistic)
*/
encoded_string_buffer = new unsigned char[content.string_length * UTF8_LONGEST_DECOMPOSITION_LEN];
encoded_string_pos = encoded_string_buffer;

/*
	Apply UTF-8 decomposition/normalization to the string (for case folding, etc) 
*/

// Leading spaces are not significant
prev_char_was_space = 1;

while (content.string_length > 0 && (character = utf8_to_wide(content.start)) != 0)
	{
	unsigned char * char_buffer_pos = char_decomposition_buffer;
	size_t char_buffer_remain = sizeof(char_decomposition_buffer);

	assert(ANT_UNICODE_normalize_lowercase_toutf8(&char_buffer_pos, &char_buffer_remain, character));

	for (int i = 0; i < (char_buffer_pos - char_decomposition_buffer); i++)
		{
		/*
			Apply provided character encoding, so we can filter out the characters that we don't
			care about the ordering of.
		 */
		unsigned char encoded = restricted ? ANT_encode_char_base37::encode(char_decomposition_buffer[i]) : ANT_encode_char_printable_ascii::encode(char_decomposition_buffer[i]);
		
		if (encoded != CHAR_ENCODE_FAIL)
			{
			if (encoded == encoded_space)
				if (prev_char_was_space)
					continue; //Strip multiple spaces in a row
				else
					prev_char_was_space = 1;
			else
				prev_char_was_space = 0;

			*encoded_string_pos = encoded;
			encoded_string_pos++;
			}
		}
	
	content.string_length -= utf8_bytes(content.start);
	content.start += utf8_bytes(content.start);
	}

/*
	Make a copy of that processed string in our memory pool
*/
result.string_length = encoded_string_pos - encoded_string_buffer;
result.start = (char *) malloc(result.string_length);
memcpy(result.start, encoded_string_buffer, result.string_length);

add_exact_string(result);

delete[] encoded_string_buffer;
}

/*
	ANT_PREGEN_WRITER_EXACT_STRINGS::CLOSE_WRITE()
	----------------------------------------------
*/
void ANT_pregen_writer_exact_strings::close_write()
{
/*
	Start the rsv's at 1
*/
ANT_pregen_t rsv = 1;

/*
	Sort the documents in order based on their exact source values, use that order
	to define the RSV of each document
*/
std::sort(&exact_strings[0], &exact_strings[doc_count], exact_str_less);

for (unsigned int i = 0; i < doc_count; i++)
	{
	/*
		Don't increase RSV if this string is the same as the previous (search engine will tiebreak)
	*/
	if (i > 0 && exact_str_less(exact_strings[i - 1], exact_strings[i]) != 0)
		rsv++;
	file.write((unsigned char *)&rsv, sizeof(rsv));
	}

ANT_pregen_writer::close_write();
}
