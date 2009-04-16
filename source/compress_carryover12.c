/*
	COMPRESS_CARRYOVER12.C
	----------------------
	Functions for Vo Ngoc Anh and Alistair Moffat's Carryover-12 compression scheme
	This is a port of Anh and Moffat's code to ANT.

	Originally (http://www.cs.mu.oz.au/~alistair/carry/)
		Copyright (C) 2003  Authors: Vo Ngoc Anh & Alistair Moffat

		This program is free software; you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		the Free Software Foundation; either version 2 of the License, or
		(at your option) any later version.

		This program is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.

	These Changes
		Copyright (C) 2006, 2009 Authors: Andrew Trotman
		GNU General Public License of this code is maintained (of course).
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compress_carryover12.h"


/* ========================================================
 Coding variables:
   trans_B1_30_big[], trans_B1_32_big are left and right transition
     tables (see the paper) for the case when the largest elements 
     occupies more than 16 bits.
   trans_B1_30_small[], trans_B1_32_small are for the otherwise case

   __pc30, __pc32 is points to the left, right tables currently used
   __pcbase points to either __pc30 or __pc32 and represents the
     current transition table used for coding
   ========================================================
*/ 

unsigned char *__pc30, *__pc32;	/* point to transition table, 30 and 32 data bits */
unsigned char *__pcbase;    /* point to current transition table */
/*
	big is transition table for the cases when number of bits
	needed to code the maximal value exceeds 16.
	_small are used otherwise.
*/
unsigned char trans_B1_30_big[]={
	0,0,0,0, 1,2,3,28, 1,2,3,28, 2,3,4,28, 3,4,5,28, 4,5,6,28,
	5,6,7,28, 6,7,8,28, 6,7,10,28, 8,10,15,28, 9,10,14,28,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 10,15,16,28, 10,14,15,28,
	7,10,15,28, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	6,10,16,28, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 4,9,15,28};

unsigned char trans_B1_32_big[]={
	0,0,0,0, 1,2,3,28, 1,2,3,28, 2,3,4,28, 3,4,5,28, 4,5,6,28,
	5,6,7,28, 6,7,8,28, 7,9,10,28, 7,10,15,28, 8,10,15,28,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 7,10,15,28, 10,15,16,28,
	10,14,15,28, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	6,10,16,28, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 4,10,16,28};

unsigned char trans_B1_30_small[]={
	0,0,0,0, 1,2,3,16, 1,2,3,16, 2,3,4,16, 3,4,5,16, 4,5,6,16,
	5,6,7,16, 6,7,8,16, 6,7,10,16, 7,8,10,16, 9,10,14,16, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 8,10,15,16, 10,14,15,16,  7,10,15,16, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 3,7,10,16};

unsigned char trans_B1_32_small[] = {
	0,0,0,0, 1,2,3,16, 1,2,3,16, 2,3,4,16, 3,4,5,16, 4,5,6,16,
	5,6,7,16, 6,7,8,16, 7,9,10,16, 7,10,15,16, 8,10,15,16, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 7,10,15,16, 8,10,15,16, 10,14,15,16, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 3,7,10,16};
 
unsigned char CLOG2TAB[] = {
	0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

#define GET_TRANS_TABLE(avail) avail < 2? (avail = 30, __pc30) : (avail -= 2, __pc32)

/*
	__MASK
	------
	__mask[i] is 2^i-1
*/
unsigned __mask[33]= {
  0x00U, 0x01U, 0x03U, 0x07U, 0x0FU,
  0x1FU, 0x3FU, 0x7FU, 0xFFU,
  0x01FFU, 0x03FFU, 0x07FFU, 0x0FFFU,
  0x1FFFU, 0x3FFFU, 0x7FFFU, 0xFFFFU,
  0x01FFFFU, 0x03FFFFU, 0x07FFFFU, 0x0FFFFFU,
  0x1FFFFFU, 0x3FFFFFU, 0x7FFFFFU, 0xFFFFFFU,
  0x01FFFFFFU, 0x03FFFFFFU, 0x07FFFFFFU, 0x0FFFFFFFU,
  0x1FFFFFFFU, 0x3FFFFFFFU, 0x7FFFFFFFU, 0xFFFFFFFFU }; 

/* 
	MACROS FOR WORD ENCODING
	========================
*/
#define WORD_ENCODE_WRITE													\
	do																		\
		{																	\
		unsigned word;														\
		word = __values[--__pvalue];											\
		for (--__pvalue; __pvalue >= 0; __pvalue--)							\
			{																\
			word <<= __bits[__pvalue];										\
			word |= __values[__pvalue];										\
			}																\
		*((unsigned *)destination) = word;									\
		destination += sizeof(word);										\
		__wremaining = 32;													\
		__pvalue = 0;														\
		}																	\
	while(0)

#define WORD_ENCODE(x,b)													\
	do 																		\
		{																	\
		if (__wremaining < (b))												\
			WORD_ENCODE_WRITE;												\
		__values[__pvalue] = (x) - 1;										\
		__bits[__pvalue++] = (b);											\
		__wremaining -= (b);												\
		} 																	\
	while (0)

#define CARRY_BLOCK_ENCODE_START(n,max_bits)								\
	do 																		\
		{																	\
		__pc30 = max_bits <= 16 ? trans_B1_30_small : trans_B1_30_big;		\
		__pc32 = max_bits <= 16 ? trans_B1_32_small : trans_B1_32_big;		\
		__pcbase = __pc30;													\
		WORD_ENCODE((max_bits <= 16 ? 1 : 2), 1);							\
		} 																	\
	while(0)

/*
	QCEILLOG_2()
	------------
*/
inline int qceillog_2(int x)
{
int _B_x  = x - 1;

return _B_x >> 16 ? (_B_x >> 24 ? 24 + CLOG2TAB[_B_x >> 24] : 16 | CLOG2TAB[_B_x >> 16]) : (_B_x >> 8 ? 8 + CLOG2TAB[_B_x >> 8] : CLOG2TAB[_B_x]);
}

/*
	CALC_MIN_BITS()
	---------------
	bits[i] = bits needed to code gaps[i]
	return max(bits[i])
*/
int calc_min_bits(unsigned *gaps, unsigned char *bits, int n)
{
int i;
int max=0;

for (i = 0; i < n; i++)
	{ 
#ifdef ALLOW_ZERO
	bits[i] = qceillog_2(gaps[i] + 1);
#else
	bits[i] = qceillog_2(gaps[i]);
#endif
	if (max<bits[i])
		max = bits[i];
	if (bits[i] > 28)
		exit(fprintf(stderr, "Error: At gap %d exceeds 2^28.\n", i));
	}
if (max > 28)
	exit(fprintf(stderr, "Error: At least one gap exceeds 2^28. It cannot be coded by this method. Terminated.\n"));

return max;
}

/*
	ELEMS_CODED()
	-------------
	given codeleng of "len" bits, and "avail" bits available for coding,
	bits[] - sequence of sizes   
	Return number_of_elems_coded (possible) if "avail" bits can be used to
	code the number of elems  with the remaining < "len"
	Returns 0 (impossible) otherwise
*/
int elems_coded(int avail, int len, unsigned char *bits, int start, int end)
{
int i, real_end, max;

if (len)
	{
	max = avail/len;
	real_end = start + max - 1 <= end ? start + max: end + 1; 
	for (i = start; i < real_end && bits[i] <= len; i++);
	if (i < real_end)
		return 0;
	return real_end - start;
	}
else
	{
	for (i = start; i < start + MAX_ELEM_PER_WORD && i <= end && bits[i] <= len; i++);			// empty loop
	if (i - start < 2)
		return 0;
	return i - start;
	}  
}

/*
	CARRY_ENCODE_BUFFER()
	---------------------
	Parameters
	a - the d-gaps (or otherwise) buffer to encode
	n - length of a (in integers)
	destination - where the compressed sequence is put
	bits - an array of bytes of length n (used as scratch space)

	returns length of destination (length in bytes)
*/
int carry_encode_buffer(unsigned *a, unsigned n, unsigned char *bits, unsigned char *destination)
{
unsigned max_bits, i, __values[32], __bits[32];
int elems, j, __wremaining = 32, __pvalue = 0, size, avail;
unsigned char *table, *base, *original_destination = destination;

size = TRANS_TABLE_STARTER;

max_bits = calc_min_bits(a,bits,n);
CARRY_BLOCK_ENCODE_START(n, max_bits);

for (i = 0; i < n; )
	{
	avail = __wremaining;
	table = GET_TRANS_TABLE(avail);
	base = table + (size << 2);       /* row in trans table */

	/* 1. Modeling: Find j= the first-fit column in base */	
	for (j = 0; j < 4; j++)
		{
		size = base[j];
		if (size > avail) 		/* must use next word for data  */
			{
			avail = 32;
			j = -1;
			continue;
			}
		if (elems = elems_coded(avail, size, bits, i, n - 1))
			break;
		}

	/* 2. Coding: Code elements using row "base" & column "j" */
	WORD_ENCODE(j + 1, 2);             /* encoding column */
	for ( ; elems ; elems--, i++)   /* encoding d-gaps */
#ifdef ALLOW_ZERO
		WORD_ENCODE(a[i] + 1, size);
#else
		WORD_ENCODE(a[i], size);
#endif

	}

if (__pvalue)
	WORD_ENCODE_WRITE;

return destination - original_destination;
}

