#include <cstdio>
#include <stdlib.h>

#include "../source/arithmetic_coding.h"

#define NUM_SYMBOLS (26 + 10 + 1)

typedef uint32_t accumulator_t;

int main(int argc, char**argv)
{
ANT_arithmetic_model acm1(NUM_SYMBOLS, NULL, 0);
ANT_arithmetic_encoder<accumulator_t> ace1(&acm1);
int sym, i;
FILE *input;
char c;

input = fopen("input", "rb");

if (!input)
	{
	fprintf(stderr,"Error\n");
	return 0;
	}

const char * encode_me = argc > 1 ? argv[1] : "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

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

accumulator_t encoded = ace1.done();
printf("Encoded as %d-bit integer: %lu\n", sizeof(encoded) * CHAR_BIT, encoded);

ANT_arithmetic_decoder<accumulator_t> acd1(&acm1, encoded);

printf("Decoded: ");

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

printf("\n");

return 0;
}

