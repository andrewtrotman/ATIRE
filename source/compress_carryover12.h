/*
	COMPRESS_CARRYOVER12.H
	----------------------
*/
#ifndef __COMPRESS_CARRYOVER12_H__
#define __COMPRESS_CARRYOVER12_H__

extern int carry_encode_buffer(unsigned *a, unsigned n, unsigned char *bits /* tmp buffer pass in and of a's length */ , unsigned char *destination);
extern int carry_decode_buffer(unsigned *destination, unsigned *source, unsigned length);


#endif __COMPRESS_CARRYOVER12_H__

