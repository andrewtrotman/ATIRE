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

Lookup_nine tbl_nine[9] = {    //look-up table to map the no. of digits into 
		{28, 1, 0x1},                           //no. of bits used for compression
		{14, 2, 0x3},
		{9,  3, 0x7},
		{7,  4, 0xF},
		{5,  5, 0x1F},
		{4,  7, 0x7F},
		{3,  9, 0x1FF},
		{2, 14, 0x3FFF},
		{1, 28, 0xFFFFFFF}
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

//this function compresses the d[] array	
long CompressRel10(long a[], long size, long n[])
{
	long j,pos=0,start,temp,k, *d;
	long r,m=0,i,row,noOfDigits,noOfBits;

	d = a;

	for(i=0;i<9;i++)  //inner loop: for flagging the table row
	{
		if(DoesHighestFit(d, 0,tbl_nine[i].noDig,tbl_nine[i].noBit,size))  //returns true or false
		{
			row = i;  
			noOfDigits = tbl_nine[i].noDig;
			noOfBits = tbl_nine[i].noBit;
			pos = noOfDigits;  //updates the position
			break;
		}
	}

	n[0] = row;  //new byte of n[] 
	n[0] = n[0] << 28;   //puts the row no. to the first 4 bits.

	for (k=0;k<pos && k < size;k++)  //puts the next noOfDigits of d[] into 1 word n[j]
	{
		temp = d[k] << (m*noOfBits);  //left shift the bits to the correct position in n[j]
		m++;    //1 digit is filled, filling done in reverse order.
		n[0] |= temp;  //bitwise OR operator
	}        

	for(j=1;pos<size;j++)  //outer loop: loops thru' all the elements in d[]
	{
		n[j] = 0;
		for(i=0;i<10;i++)  //inner loop: for flagging the table row
		{
			if(tbl_ten[row].transferArray[i] == 9)   //invalid rows
				continue;
	
			if(DoesHighestFit(d,pos,tbl_ten[i].noDig,tbl_ten[i].noBit, size))  //returns true or false
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

		n[j] = r;  //new byte of n[] 
		n[j] = n[j] << 30;   //puts the row no. to the first 2 bits.

		m=0;

		for (k = start;k < pos && k < size;k++)  //puts the next noOfDigits of d[] into 1 word n[j]
		{
			temp = d[k] << (m*noOfBits);  //left shift the bits to the correct position in n[j]
			m++;    //1 digit is filled, filling done in reverse order.
			n[j] |= temp;  //bitwise OR operator
		}
	}
	return j;  //stores the length of n[]
}

