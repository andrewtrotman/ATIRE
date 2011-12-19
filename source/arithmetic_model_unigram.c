/*
	ARITHMETIC_MODEL_UNIGRAM.C
	--------------------------
	Derived from http://www.cipr.rpi.edu/~wheeler/ac/. Modified to encode/decode to a fixed-sized
	integer instead of a file, and translate to C++
*/
#include <cstdlib>
#include <cstdio>
#include <cassert>

#undef max
#undef min

#include "arithmetic_model_unigram.h"

#define MAX_FREQUENCY 16383

/*
	ANT_ARITHMETIC_MODEL_UNIGRAM::ANT_ARITHMETIC_MODEL_UNIGRAM()
	------------------------------------------------------------
*/
ANT_arithmetic_model_unigram::ANT_arithmetic_model_unigram(int nsym, const unsigned int *ifreq, int adapt)
{
int i;

this->nsym = nsym;
this->adapt = adapt;

freq = new unsigned int[nsym];
cfreq = new unsigned int[nsym + 1];

assert(freq && cfreq);

if (ifreq)
	{
	cfreq[nsym] = 0;
	for (i = nsym - 1; i >= 0; i--)
		{
		freq[i] = ifreq[i];
		cfreq[i] = cfreq[i + 1] + freq[i];
		}
	if (cfreq[0] > MAX_FREQUENCY)
		{
		fprintf(stderr, "arithmetic coder model max frequency exceeded");
		exit(-1);
		}
	}
else
	{
	for (i = 0; i < nsym; i++)
		{
		freq[i] = 1;
		cfreq[i] = nsym - i;
		}
	cfreq[nsym] = 0;
	}
}

/*
	ANT_ARITHMETIC_MODEL_UNIGRAM::~ANT_ARITHMETIC_MODEL_UNIGRAM()
	-------------------------------------------------------------
*/
ANT_arithmetic_model_unigram::~ANT_arithmetic_model_unigram()
{
delete [] freq;
delete [] cfreq;
}

/*
	ANT_ARITHMETIC_MODEL_UNIGRAM::TEXT_RENDER()
	-------------------------------------------
*/
void ANT_arithmetic_model_unigram::text_render(void)
{
int col = 0;

for (int i = 0; i < nsym; i++)
	{
	printf(i == nsym - 1 ? "%d" : "%d, ", freq[i]);

	col++;
	if (col % 16 == 0)
		printf("\n");
	}
printf("\n");
}

/*
	ANT_ARITHMETIC_MODEL_UNIGRAM::UPDATE()
	--------------------------------------
*/
void ANT_arithmetic_model_unigram::update(int sym)
{
int i;

if (!adapt)
	return;

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
