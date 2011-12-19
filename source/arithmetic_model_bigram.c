/*
	ARITHMETIC_MODEL_BIGRAM.C
	-------------------------
	Derived from http://www.cipr.rpi.edu/~wheeler/ac/. Modified to encode/decode to a fixed-sized
	integer instead of a file, and translate to C++
*/

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <string.h>

#undef max
#undef min

#include "arithmetic_model_bigram.h"

#define MAX_FREQUENCY 16383

/*
	ANT_ARITHMETIC_MODEL_BIGRAM::ANT_ARITHMETIC_MODEL_BIGRAM()
	----------------------------------------------------------
*/
ANT_arithmetic_model_bigram::ANT_arithmetic_model_bigram(int nsym, const unsigned int *ifreq, int adapt)
{
int i, j;

this->nsym = nsym;
this->adapt = adapt;

this->prev_symbol = 0;

bigram_freq = new unsigned int[(nsym + 1) * (nsym)];
bigram_cfreq = new unsigned int[(nsym + 1) * (nsym + 1)];

assert(bigram_freq && bigram_cfreq);

if (ifreq)
	{
	for (int context = 0; context < nsym + 1; context++)
		{
		set_context(context);

		cfreq[nsym] = 0;
		for (i = nsym - 1; i >= 0; i--)
			{
			freq[i] = ifreq[context * nsym + i];
			cfreq[i] = cfreq[i + 1] + freq[i];
			}
		if (cfreq[0] > MAX_FREQUENCY)
			{
			fprintf(stderr, "arithmetic coder model max frequency exceeded");
			exit(-1);
			}
		}
	}
else
	{
	//For each initial symbol, including the "start of string" symbol
	for (i = 0; i < nsym + 1; i++)
		{
		//For each probability of a following symbol..
		for (j = 0; j < nsym; j++)
			{
			bigram_freq[i * nsym + j] = 1;
			bigram_cfreq[i * (nsym + 1) + j] = nsym - i;
			}
		bigram_cfreq[i * (nsym + 1) + nsym] = 0;
		}
	}

/* We begin in the "start of string" context */
clear_context();
}

/*
	ANT_ARITHMETIC_MODEL_BIGRAM()
	-----------------------------
*/
ANT_arithmetic_model_bigram::~ANT_arithmetic_model_bigram()
{
delete[] bigram_freq;
delete[] bigram_cfreq;
}

/*
	ANT_ARITHMETIC_MODEL_BIGRAM::SET_CONTEXT()
	------------------------------------------
*/
void ANT_arithmetic_model_bigram::set_context(int symplusone)
{
prev_symbol = symplusone;
cfreq = &(bigram_cfreq[prev_symbol * (nsym + 1)]);
freq = &(bigram_freq[prev_symbol * (nsym)]);
}

/*
	ANT_ARITHMETIC_MODEL_BIGRAM::UPDATE_CONTEXT()
	---------------------------------------------
*/
void ANT_arithmetic_model_bigram::update_context(int sym)
{
int i;

if (cfreq[0] == MAX_FREQUENCY)
	{
	int cum = 0;
	cfreq[nsym] = 0;
	for (i = nsym - 1; i >= 0; i--)
		{
		freq[i] = (freq[i] + 1) / 2;
		cum += freq[i];
		cfreq[i] = cum;
		}
	}

freq[sym] += 1;
for (i = sym; i >= 0; i--)
	cfreq[i] += 1;
}

/*
	ANT_ARITHMETIC_MODEL_BIGRAM::UPDATE()
	-------------------------------------
*/
void ANT_arithmetic_model_bigram::update(int sym)
{
if (adapt)
	{
	/* Update the order-2 model for this symbol following the previous symbol.. */
	update_context(sym);

	if (prev_symbol != 0)
		{
		/* Also update the order-1 context.. */
		clear_context();
		update_context(sym);
		}
	}

/* Now this symbol becomes the last-seen symbol */
set_context(sym + 1);
}

/*
	ANT_ARITHMETIC_MODEL_BIGRAM::CLEAR_CONTEXT()
	--------------------------------------------
*/
void ANT_arithmetic_model_bigram::clear_context(void)
{
set_context(0);
}

/*
	ANT_ARITHMETIC_MODEL_BIGRAM::TEXT_RENDER()
	------------------------------------------
*/
void ANT_arithmetic_model_bigram::text_render(void)
{
int col = 0;

for (int i = 0; i < nsym + 1; i++)
	for (int j = 0; j < nsym; j++)
		{
		printf((i < nsym || j < nsym -1 ? "%d, " : "%d"), bigram_freq[i * nsym + j]);

		col++;
		if (col % 16 == 0)
			printf("\n");
		}
printf("\n");
}

