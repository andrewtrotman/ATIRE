/*
	PREGEN_WRITER_EXACT_INTEGERS.C
	------------------------------
*/
#include <assert.h>
#include <algorithm>
#include "maths.h"
#include "pregen_t.h"
#include "pregen_writer_exact_integers.h"
#include "encode_char_base37.h"
#include "encode_char_printable_ascii.h"
#include "unicode.h"


/*
	ANT_PREGEN_WRITER_EXACT_INTEGERS::ANT_PREGEN_WRITER_EXACT_INTEGERS()
	--------------------------------------------------------------------
*/
ANT_pregen_writer_exact_integers::ANT_pregen_writer_exact_integers(const char *name) : ANT_pregen_writer(INTEGEREXACT, name)
{
doc_capacity = 1024;
exact_integers = new std::pair<long long,long long>[doc_capacity];
}

/*
	ANT_PREGEN_WRITER_EXACT_INTEGERS::~ANT_PREGEN_WRITER_EXACT_INTEGERS()
	---------------------------------------------------------------------
*/
ANT_pregen_writer_exact_integers::~ANT_pregen_writer_exact_integers()
{
delete [] exact_integers;
}

/*
	EXACT_INT_LESS()
	----------------
*/
static bool exact_int_less(const std::pair<long long, long long>& a, const std::pair<long long, long long>& b)
{
return a.second < b.second;
}

/*
	ANT_PREGEN_WRITER_EXACT_INTEGERS::ENSURE_STORAGE()
	--------------------------------------------------
*/
void ANT_pregen_writer_exact_integers::ensure_storage()
{
if (doc_count >= doc_capacity)
	{
	doc_capacity = doc_capacity * 2 + 1;

	std::pair<long long, long long> *new_integers = new std::pair<long long, long long>[doc_capacity];
	memcpy(new_integers, exact_integers, sizeof(exact_integers[0]) * doc_count);

	delete [] exact_integers;
	exact_integers = new_integers;
	}
}

/*
	ANT_PREGEN_WRITER_EXACT_INTEGERS::ADD_EXACT_INTEGER()
	-----------------------------------------------------
*/
void ANT_pregen_writer_exact_integers::add_exact_integer(long long i)
{
ensure_storage();

exact_integers[doc_count].first = doc_count;
exact_integers[doc_count].second = i;

doc_count++;
}

/*
	ANT_PREGEN_WRITER_EXACT_INTEGERS::ADD_FIELD()
	---------------------------------------------
*/
void ANT_pregen_writer_exact_integers::add_field(long long  docindex, ANT_string_pair content)
{
long long skip_docs;

/*
	It's possible that previous documents didn't contain this field, so be prepared to zero-pad.
*/
skip_docs = docindex - doc_count;

assert(skip_docs >= 0);

while (skip_docs > 0)
	{
	add_exact_integer(0);
	skip_docs--;
	}

//TODO long long conversion
add_exact_integer(atol(content.start));
}

/*
	ANT_PREGEN_WRITER_EXACT_INTEGERS::CLOSE_WRITE()
	-----------------------------------------------
*/
void ANT_pregen_writer_exact_integers::close_write()
{
ANT_pregen_t rsv;

/* 
	Sort the documents in order based on their exact source values, use that order
	to define the RSV of each document 
*/
std::sort(&exact_integers[0], &exact_integers[doc_count], exact_int_less);

/* Each document's RSV is its position in the ranking (also add 1 to avoid generating a
 * zero RSV) */
for (unsigned int i = 0; i < doc_count; i++)
	file.write((unsigned char *)&(rsv = i + 1), sizeof(rsv));

ANT_pregen_writer::close_write();
}
