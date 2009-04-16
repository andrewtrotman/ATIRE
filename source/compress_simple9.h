
/*
	COMPRESS_SIMPLE9.H
	------------------
*/
#ifndef __COMPRESS_SIMPLE9_H__
#define __COMPRESS_SIMPLE9_H__

long CompressSim9(long a[], long size, long n[]);
void InitSim9(void);

typedef struct {
	long noDig;
	long noBit;
	long shft;
}Lookup;

extern Lookup tbl[9];

/*
	DECOMPRESSSIM9()
	----------------
	this function decompresses the n[] array to get back differences in p[]	
*/
inline void DecompressSim9(long *n, long compressedLength, long *into)
{
	long j=0,temp1,row1, shift;
	long noOfDigits1,noOfBits1;

	for(j=0;j<compressedLength;j++)  //outer loop: loops thru' all the compressed elements of n[]
	{
		temp1 = *n++;
		row1 = (unsigned)temp1 >> 28;  //row no. is got by eliminating the last 28 bits
		temp1 &= 0x0fffffff;

		noOfBits1 = tbl[row1].noBit;   //gets info. from the look-up table
		shift = tbl[row1].shft;
		noOfDigits1 = tbl[row1].noDig;
		
		for(; noOfDigits1 > 0; noOfDigits1--) //extracts "noOfDigits" digits from 1 compressed word n[j]
		{
			*into++ = temp1 & shift;      //puts the digit in p[i]
			temp1 >>= noOfBits1;    //shifting appropriately
		}
	}
}



#endif __COMPRESS_SIMPLE9_H__
