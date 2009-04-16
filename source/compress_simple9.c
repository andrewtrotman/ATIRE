/*
	COMPRESS_SIMPLE9.C
	------------------
*/
/* 
	Coded by Vikram S under the guidance of Andrew Trotman, University of Otago, New Zealand.

	Implementation of the Simple-9 compression algorithm described in the Information Retrieval paper
	"Inverted Index Compression Using Word-Alligned Binary Codes" by Anh and Moffat in 2004.
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "compress_simple9.h"

#define FALSE 0
#define TRUE (!FALSE)


Lookup tbl[9] = {
		{28,1, 0x1},                           //no. of bits used for compression
		{14,2, 0x3},
		{9,3, 0x7},
		{7,4, 0xF},
		{5,5, 0x1F},
		{4,7, 0x7F},
		{3,9, 0x1FF},
		{2,14, 0x3FFF},
		{1,28, 0xFFFFFFF}
	};

/*
	DOESHIGHESTFIT()
	----------------
	This function checks whether the highest no. in the 
	range d[pos] to d[pos + noOfDigits] can fit in "noOfBits" bits
*/
static long DoesHighestFit(long d[], long pos, long noOfDigits, long noOfBits, long size)
{
	long i = 0,highest = 0;
	highest = d[pos];

	for(i=pos+1; i<(pos + noOfDigits) && i<size ; i++)
	{
		if(highest<d[i])
			highest = d[i];   //stores the highest no. in "highest"
	}

	if(highest < (((unsigned long)1) << noOfBits))  //checks if "highest" fits in noOfBits
		return TRUE;
	else 
		return FALSE;
}

/*
	COMPRESSSIM9()
	--------------
	this function compresses the d[] array	
*/
long CompressSim9(long a[], long size, long n[])
{
	long m=0,j=0,pos=0,start=0,temp=0,k=0, *d;// bitLength = 0;
	long noOfDigits=0,noOfBits=0, row=0,i=0;
	//d[] holds the difference of adjacent numbers in a[], n[] is 
										//the new compressed array and is decompressed into p[]
	d = a;
	
	for(j=0;pos<size;j++)  //outer loop: loops thru' all the elements in d[]
	{
		for(i=0;i<9;i++)  //inner loop: for flagging the table row
		{
			if(DoesHighestFit(d, pos,tbl[i].noDig,tbl[i].noBit,size))  //returns true or false
			{
				start = pos;    //marks the starting position, row, noOfDigits, noOfBits required for each
				row = i;  
				noOfDigits = tbl[i].noDig;
				noOfBits = tbl[i].noBit;
				pos = pos + noOfDigits;  //updates the position
				break;
			}
		}
	
		n[j] = row;  //new byte of n[] 
		n[j] = n[j] << 28;   //puts the row no. to the first 4 bits.
		//bitLength += 4;

		m=0;

		for (k=start;k<pos && k < size;k++)  //puts the next noOfDigits of d[] into 1 word n[j]
		{
			temp = ((unsigned long)d[k]) << (m*noOfBits);  //left shift the bits to the correct position in n[j]
			m++;    //1 digit is filled, filling done in reverse order.
			n[j] |= temp;  //bitwise OR operator
		}
	}

	return j;  //stores the length of n[]
}
