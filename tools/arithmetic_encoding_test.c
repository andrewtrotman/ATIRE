#include <cstdio>
#include <stdlib.h>

#include "../source/arithmetic_coding.h"

#define NUM_SYMBOLS (26 + 10 + 1)

int main(void)
{
ac_encoder ace1;
ac_decoder acd1;
ac_model acm1;
int sym, i;
FILE *input;
char c;

input = fopen("input", "rb");

if (!input)
	{
	fprintf(stderr,"Error\n");
	return 0;
	}

ac_encoder_init(&ace1);
ac_model_init(&acm1, NUM_SYMBOLS, NULL, 0);

const char * encode_me = "ABC";

while ((c = *encode_me))
	{
	int symbol;

	if (c == ' ')
		symbol = 0;
	else if (c >= '0' && c <= '9')
		symbol = c - '0' + 1;
	else if (c >= 'a' && c <= 'z')
		symbol = c - 'a' + 1 + 10;
	else if (c >= 'A' && c <= 'Z')
		symbol = c - 'A' + 1 + 10;
	else continue;

	ac_encode_symbol(&ace1, &acm1, symbol);

	encode_me++;
	}


ac_encoder_done(&ace1);
ac_model_done(&acm1);

printf("bits for encoder 1: %d\n", (int) ac_encoder_bits(&ace1));

printf("%lu\n", ace1.buffer);

ac_decoder_init(&acd1, ace1.buffer);
ac_model_init(&acm1, NUM_SYMBOLS, NULL, 0);

while (acd1.bits_to_go)
	{
	sym = ac_decode_symbol(&acd1, &acm1);
	if (sym==0)
		printf(" ");
	else if (sym >= 1 && sym <= 10)
		printf("%c", sym - 1 + '0');
	else
		printf("%c", sym - 1 - 10 + 'a');
	}

ac_decoder_done(&acd1);
ac_model_done(&acm1);

return 0;
}

