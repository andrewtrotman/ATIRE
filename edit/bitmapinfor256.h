/*
	BITMAPINFOR256.H
	----------------
*/
#ifndef BITMAPINFOR256_H_
#define BITMAPINFOR256_H_

#include <windows.h>

/*
        BITMAPSINFO256
        --------------
*/
typedef struct
{
BITMAPINFOHEADER bmiHeader;
RGBQUAD bmiColors[256];         // fake a 256 colour bitmap (we only use  40 of them, but want a byte-per-pixel frame buffer)
} BITMAPINFO256;

#endif /* BITMAPINFOR256_H_ */
