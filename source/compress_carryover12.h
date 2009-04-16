/*
	COMPRESS_CARRYOVER12.H
	----------------------
*/
#ifndef __COMPRESS_CARRYOVER12_H__
#define __COMPRESS_CARRYOVER12_H__

extern int carry_encode_buffer(unsigned *a, unsigned n, unsigned char *bits /* tmp buffer pass in and of a's length */ , unsigned char *destination);
extern int carry_decode_buffer(unsigned *destination, unsigned *source, unsigned length);

#define ALLOW_ZERO				1

#define MAX_ELEM_PER_WORD		64
#define TRANS_TABLE_STARTER	33

extern unsigned char *__pc30, *__pc32;	/* point to transition table, 30 and 32 data bits */
extern unsigned char *__pcbase;    /* point to current transition table */

extern unsigned char trans_B1_30_big[];
extern unsigned char trans_B1_32_big[];
extern unsigned char trans_B1_30_small[];
extern unsigned char trans_B1_32_small[];
extern unsigned char CLOG2TAB[];
extern unsigned __mask[33];

/*
	MACROS FOR WORD DECODING
	========================
*/
#define GET_NEW_WORD														\
	__wval= *__wpos++
  
#define WORD_DECODE(x,b)													\
	do 																		\
		{																	\
		if (__wremaining < (b))												\
			{  																\
			GET_NEW_WORD;													\
			__wremaining = 32;												\
			}																\
		(x) = (__wval & __mask[b]) + 1;										\
		__wval >>= (b);														\
		__wremaining -= (b);												\
		} 																	\
	while (0)

#define CARRY_BLOCK_DECODE_START											\
do  																		\
	{																		\
	int tmp;																\
	WORD_DECODE(tmp, 1);													\
	__pc30 = tmp == 1 ? trans_B1_30_small : trans_B1_30_big;				\
	__pc32 = tmp == 1 ? trans_B1_32_small : trans_B1_32_big;				\
	__pcbase = __pc30;														\
	CARRY_DECODE_GET_SELECTOR												\
	} 																		\
while (0)

#define CARRY_DECODE_GET_SELECTOR											\
	if (__wremaining >= 2)													\
		{																	\
		__pcbase = __pc32;													\
		__wbits = __pcbase[(__wbits << 2) + (__wval & 3)];					\
		__wval >>= 2;														\
		__wremaining -= 2;													\
		if (__wremaining < __wbits)											\
			{																\
			GET_NEW_WORD;													\
			__wremaining = 32;												\
			}																\
		}																	\
	else																	\
		{																	\
		__pcbase = __pc30;													\
		GET_NEW_WORD;														\
		__wbits = __pcbase[(__wbits << 2) + (__wval & 3)];					\
		__wval >>= 2;														\
		__wremaining = 30; 													\
		}

#ifdef ALLOW_ZERO
	#define CARRY_DECODE(x)													\
		do 																	\
			{																\
			if (__wremaining < __wbits)										\
			CARRY_DECODE_GET_SELECTOR										\
			x = (__wval & __mask[__wbits]);								\
			__wval >>= __wbits;												\
			__wremaining -= __wbits;										\
			}																\
		while(0)
#else
	#define CARRY_DECODE(x)													\
		do 																	\
			{																\
			if (__wremaining < __wbits)										\
			CARRY_DECODE_GET_SELECTOR										\
			x = (__wval & __mask[__wbits]) + 1;							\
			__wval >>= __wbits;												\
			__wremaining -= __wbits;										\
			}																\
		while(0)
#endif

/*
	CARRY_DECODE_BUFFER()
	---------------------
	__wpos is the compressed string
	destination is the destination
	n is the number of unsigneds in __wpos
*/
inline int carry_decode_buffer(unsigned *destination, unsigned *__wpos, unsigned n)
{
unsigned i;
unsigned curr = 0;
int __wbits = TRANS_TABLE_STARTER;
int __wremaining = -1;
unsigned __wval = 0;

CARRY_BLOCK_DECODE_START;
for (i = 0; i < n; i++)
	CARRY_DECODE(*destination++);

return 1;
}

#endif __COMPRESS_CARRYOVER12_H__
