/*
	COMPRESS_RELATIVE10.C
	---------------------
*/
/* 
	Coded by Vikram S under the guidance of Andrew Trotman, University of Otago, New Zealand.

  Implementation of the Relative-10 compression algorithm described in the Information Retrieval paper
  "Inverted Index Compression Using Word-Alligned Binary Codes" by Anh and Moffat in 2004.
*/

#include "compress_relative10.h"
#include "maths.h"

typedef struct {
	long noDig;
	long noBit;
	long shft;
	long transferArray[10];
	long aspt_pos[4];
} Lookup_ten;

Lookup_ten tbl_ten[10] = {    //look-up table to map the no. of digits into 
		{30, 1, 0x1, 		0, 1, 2, 9, 9, 9, 9, 9, 9, 3,       0, 1, 2, 9},
		{15, 2, 0x3,        0, 1, 2, 9, 9, 9, 9, 9, 9, 3,		0, 1, 2, 9},
		{10, 3, 0x7,        9, 0, 1, 2, 9, 9, 9, 9, 9, 3,		1, 2, 3, 9},
		{7,  4, 0xF,        9, 9, 0, 1, 2, 9, 9, 9, 9, 3,		2, 3, 4, 9},
		{6,  5, 0x1F,       9, 9, 9, 0, 1, 2, 9, 9, 9, 3,		3, 4, 5, 9},
		{5,  6, 0x3F,       9, 9, 9, 9, 0, 1, 2, 9, 9, 3,		4, 5, 6, 9},
		{4,  7, 0x7F,       9, 9, 9, 9, 9, 0, 1, 2, 9, 3,		5, 6, 7, 9},
		{3, 10, 0x3FF,      9, 9, 9, 9, 9, 9, 0, 1, 2, 3,		6, 7, 8, 9},
		{2, 15, 0x7FFF,     9, 9, 9, 9, 9, 9, 0, 1, 2, 3,		6, 7, 8, 9},
		{1, 30, 0x3FFFFFFF, 9, 9, 9, 9, 9, 9, 0, 1, 2, 3,		6, 7, 8, 9}
	};

//This function checks whether the highest no. in the 
//range d[pos] to d[pos + noOfDigits] can fit in "noOfBits" bits
static bool DoesHighestFit(long d[], long pos, long noOfDigits, long noOfBits, long size )
{
	long i,highest;
	highest = d[pos];

	for(i=pos+1; i<(pos + noOfDigits) && i<size ; i++)
	{
		if(highest<d[i])
			highest = d[i];   //stores the highest no. in "highest"
	}

	if((unsigned long)highest< ((unsigned long)1<<noOfBits))  //checks if "highest" fits in noOfBits
		return true;

	else 
		return false;
}

/*
	ANT_COMPRESS_RELATIVE10::COMPRESS()
	-----------------------------------
*/
long long ANT_compress_relative10::compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers)
{
long *a = (long *)source;
long size = source_integers;
long *n = (long *)destination;

long j,pos=0,start,temp,k, *d;
long r,m=0,i,row,noOfDigits,noOfBits;

d = a;

/*
	Encode the first word using Simple 9
*/
long term, needed, needed_for_this_integer, bits_per_integer;
ANT_compressable_integer *from = (ANT_compressable_integer *)a;
needed = 0;
for (term = 0; term < 28 && pos + term < source_integers; term++)
	{
	needed_for_this_integer = bits_to_use[ANT_ceiling_log2(source[pos + term])];
	if (needed_for_this_integer > 28 || needed_for_this_integer < 1)
		return 0;					// we fail because there is an integer greater then 2^28 (or 0) and so we cannot pack it
	if (needed_for_this_integer > needed)
		needed = needed_for_this_integer;
	if (needed * term >= 28)				// then we'll overflow so break out
		break;
	}

row = table_row[term - 1];
pos = simple9_table[row].numbers;
bits_per_integer = simple9_table[row].bits;

n[0] = row << 28;   //puts the row no. to the first 4 bits.
for (term = 0; from < source + pos; term++)
	n[0] |= (*from++ << (term * bits_per_integer));  //left shift the bits to the correct position in n[j]

/*
	And the remainder in Relative 10
*/
for(j = 1; pos < size; j++)  //outer loop: loops thru' all the elements in d[]
	{
	n[j] = 0;
	for (i = 0; i < 10; i++)  //inner loop: for flagging the table row
		{
		if (tbl_ten[row].transferArray[i] == 9)   //invalid rows
			continue;

		if (DoesHighestFit(d, pos, tbl_ten[i].noDig, tbl_ten[i].noBit, size))  //returns true or false
			{
			start = pos; //marks the starting position, row, noOfDigits, noOfBits required for each
			r = tbl_ten[row].transferArray[i];   //row selector
			row = i;       //update the row no.
			noOfDigits = tbl_ten[i].noDig;
			noOfBits = tbl_ten[i].noBit;
			pos = pos + noOfDigits;  //updates the position
			break;
			}
		}

	n[j] = r << 30;   //puts the row no. to the first 2 bits.
	m = 0;

	for (k = start; k < pos && k < size; k++)  //puts the next noOfDigits of d[] into 1 word n[j]
		{
		temp = d[k] << (m*noOfBits);  //left shift the bits to the correct position in n[j]
		m++;    //1 digit is filled, filling done in reverse order.
		n[j] |= temp;  //bitwise OR operator
		}
	}
return j * 4;  //stores the length of n[]
}

/*
	ANT_COMPRESS_RELATIVE10::DECOMPRESS()
	-------------------------------------
*/
void ANT_compress_relative10::decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers)
{
long long numbers;
long mask, bits;
uint32_t *compressed_sequence = (uint32_t *)source;
uint32_t value, row;
ANT_compressable_integer *end = destination + destination_integers;

/*
	The first word is encoded in Simple-9
*/
value = *compressed_sequence++;
row = value >> 28;  		// row no. is got by eliminating the last 28 bits
value &= 0x0fffffff;

bits = simple9_table[row].bits;
mask = simple9_table[row].mask;
numbers = simple9_table[row].numbers;

if (numbers > destination_integers)
	numbers = destination_integers;

while (numbers-- > 0)
	{
	*destination++ = value & mask;
	value >>= bits;
	}

/*
	The remainder is in relative-10
*/
while (1)
	{
	value = *compressed_sequence++;
	row = tbl_ten[row].aspt_pos[value >> 30];
	value &= 0x3fffffff;		// top 2 bits are the relative selector, botton 30 are the integer

	bits = tbl_ten[row].noBit;
	mask = tbl_ten[row].shft;
	numbers = tbl_ten[row].noDig;

	if (destination + numbers < end)
		while (numbers-- > 0)
			{
			*destination++ = value & mask;
			value >>= bits;
			}
	else
		{
		numbers = end - destination;
		while (numbers-- > 0)
			{
			*destination++ = value & mask;
			value >>= bits;
			}
		break;
		}
	}
}

