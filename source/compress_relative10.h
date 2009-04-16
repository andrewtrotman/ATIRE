/*
	COMPRESS_RELATIVE10.H
	---------------------
*/
#ifndef __COMPRESS_RELATIVE10_H__
#define __COMPRESS_RELATIVE10_H__

long CompressRel10(long a[], long size, long n[]);
void DecompressRel10(long n[], long compressedLength, long p[], long size);

typedef struct {
	long noDig;
	long noBit;
	long shft;
	long transferArray[10];
	long aspt_pos[4];
} Lookup_ten;

typedef struct {
	long noDig;
	long noBit;
	long shft;
} Lookup_nine;

extern Lookup_ten tbl_ten[10];
extern Lookup_nine tbl_nine[9];

//this function decompresses the n[] array to get back differences in p[]	
inline void DecompressRel10(long n[], long compressedLength, long p[], long size)
{
	long r=3,i=0,j=0,temp1=0,shift=0;
	long k,noOfDigits1=0,noOfBits1=0,row1=9;
	
	temp1 = n[0];
	row1 = (unsigned)temp1 >> 28;  //row no. is got by eliminating the last 28 bits
	temp1 &= 0x0fffffff;

	noOfBits1 = tbl_nine[row1].noBit;   //gets info. from the look-up table
	shift = tbl_nine[row1].shft;
	noOfDigits1 = tbl_nine[row1].noDig;
	
	for(; noOfDigits1 > 0; noOfDigits1--) //extracts "noOfDigits" digits from 1 compressed word n[j]
	{
		p[i++] = temp1 & shift;      //puts the digit in p[i]
		temp1 >>= noOfBits1;    //shifting appropriately
	}

	for(j=1;j<compressedLength;j++)  //outer loop: loops thru' all the compressed elements of n[]
	{
		temp1 = n[j];
		r = temp1;
		r = ((unsigned long)r) >> 30;  //row no. is got by eliminating the last 28 bits

		row1 = tbl_ten[row1].aspt_pos[r];

		noOfDigits1 = tbl_ten[row1].noDig;  //gets info. from the look-up table
		noOfBits1 = tbl_ten[row1].noBit;
		shift = tbl_ten[row1].shft;

		temp1 &= 0x3fffffff;   //removes the first 2 bits.
		
		for(;noOfDigits1>0 && i<size;noOfDigits1--) //extracts "noOfDigits" digits from 1 compressed word n[j]
		{
			p[i++] = temp1 & shift;      //puts the digit in p[i]
			temp1 = ((unsigned)temp1) >> noOfBits1;    //shifting appropriately					
		}
	}
}

#endif __COMPRESS_RELATIVE10_H__
