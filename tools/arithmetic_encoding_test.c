#include <cstdio>
#include <stdlib.h>
#include <algorithm>

#include "../source/arithmetic_coding.h"
#include "../source/arithmetic_coding_encoder.h"
#include "../source/arithmetic_coding_decoder.h"
#include "../source/arithmetic_model_unigram.h"

#define NUM_SYMBOLS (26 + 10 + 1)

typedef uint64_t accumulator_t;

ANT_arithmetic_model_unigram *model;

struct encode_result {
	const char * string;
	unsigned long long encoded;
};

accumulator_t arith_encode(const char *encode_me)
{
ANT_arithmetic_coding_encoder<accumulator_t> ace1(model);

char c;

while ((c = *encode_me))
	{
	int symbol;

	encode_me++;

	if (c == ' ')
		symbol = 0;
	else if (c >= '0' && c <= '9')
		symbol = c - '0' + 1;
	else if (c >= 'a' && c <= 'z')
		symbol = c - 'a' + 1 + 10;
	else if (c >= 'A' && c <= 'Z')
		symbol = c - 'A' + 1 + 10;
	else
		continue;

	ace1.encode_symbol(symbol);
	}

return ace1.done();
}

void arith_decode(accumulator_t encoded)
{
ANT_arithmetic_decoder<accumulator_t> acd1(model, encoded);
int sym;

/* We don't know how many symbols we encoded, since we don't store it and there's
 * no terminating symbol. That's fine, we're just decoding for debugging. We'll decode
 * a "reasonable" number of symbols instead.
 */
for (int i = 0 ; i < sizeof(encoded) * CHAR_BIT; i++)
	{
	sym = acd1.decode_symbol();
	if (sym==0)
		printf(" ");
	else if (sym >= 1 && sym <= 10)
		printf("%c", sym - 1 + '0');
	else
		printf("%c", sym - 1 - 10 + 'a');
	}
}

bool compare_encode_result(const struct encode_result & a, const struct encode_result & b)
{
return a.encoded > b.encoded;
}

int main(int argc, char**argv)
{
static unsigned int symbol_frequencies[]= {
		1317, 11, 33, 11, 13, 3, 3, 1, 3, 5, 6, 719,
		92, 378, 337, 827, 143, 214, 261, 458,15, 61, 351, 219,
		510, 591, 330, 8, 593, 451, 585, 138, 68, 221, 26, 158, 6
	};

model = new ANT_arithmetic_model_unigram(NUM_SYMBOLS, symbol_frequencies, 0);

accumulator_t encoded = arith_encode(argc > 1 ? argv[1] : "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

printf("Encoded as a %d-bit integer: %llu\n", sizeof(encoded) * CHAR_BIT, (unsigned long long) encoded);

printf("Decoded: ");

arith_decode(encoded);
printf("\n");

const char *test_strings[] ={
	" ",
	"1",
	"1a",
	"a",
	"b",
	"c",
	"d",
	"e",
	"f",
	"g",
	"h",
	"i",
	"j",
	"k",
	"z",
	"zz",
	"aa",
	"ab",
	"ba",
	"bc",
	" "
};

struct encode_result result[sizeof(test_strings) / sizeof(test_strings[0])];

for (int i = 0; i < sizeof(test_strings) / sizeof(test_strings[0]); i++)
	{
	result[i].string = test_strings[i];
	result[i].encoded = (unsigned long long) arith_encode(test_strings[i]);
	}

std::sort(&result[0], &result[sizeof(test_strings) / sizeof(test_strings[0])], compare_encode_result);

for (int i = 0; i < sizeof(result) / sizeof(result[0]); i++)
	printf("'%4s' %llu\n", result[i].string, result[i].encoded);

return 0;
}

