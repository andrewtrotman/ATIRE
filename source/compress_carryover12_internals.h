/*
	COMPRESS_CARRYOVER12_INTERNALS.H
	--------------------------------
	Don't ask!  This is the internals of the carryover-12 decompression scheme
	that are also used by sigma encoding and so are exposed in this "internals"
	file.
*/
#ifndef __COMPRESS_CARRYOVER12_INTERNALS_H__
#define __COMPRESS_CARRYOVER12_INTERNALS_H__

#include "fundamental_types.h"
#include "compress_carryover12.h"

extern unsigned char *__pc30, *__pc32;
extern unsigned char trans_B1_30_big[];
extern unsigned char trans_B1_32_big[];
extern unsigned char trans_B1_30_small[];
extern unsigned char trans_B1_32_small[];
extern unsigned char *__pcbase;
extern unsigned __mask[33];

#define TRANS_TABLE_STARTER	33

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
	int32_t tmp;																\
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



#define CARRY_DECODE(x)													\
	do 																		\
		{																	\
		if (__wremaining < __wbits)											\
		CARRY_DECODE_GET_SELECTOR											\
		x = (__wval & __mask[__wbits]);										\
		__wval >>= __wbits;													\
		__wremaining -= __wbits;											\
		}																	\
	while(0)


#endif  /* __COMPRESS_CARRYOVER12_INTERNALS_H__ */



