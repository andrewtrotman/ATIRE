/*
	LOVINS.H
	--------
*/

#ifndef __LOVINS_H__
#define __LOVINS_H__

/*
	This implementation of the Lovins stemmer comes from Waikato.  It uses "Pascal" strings
	instead of 'C' strings - that is, the first byte of the string stores its length!  To 
	use this in ANT it is necesary to convert from null-terminate strings to length-encoded 
	strings (remind you of the BSTR thunking problem in OLE?).
*/
void hidden_lovins_stem (unsigned char *word);

/*
	LOVINS_STEM()
	-------------
*/
size_t Lovins_stem(unsigned char *source, unsigned char *destination)
{
long long len;

if ((len = strlen(source)) > 0xFF)
	{
	/*
		If the string is longer than 255 characters then don't stem it (because we can't store the length as a byte)
	*/
	strcpy(destination, source);
	return len;
	}

strcpy(destination + 1, source, len);
*destination = (unsigned char)len;
hidden_lovins_stem(destination);
len = *destination;
memmove(destination, destination + 1, len);
destination[len] = '\0';

return 3;
}

#endif __LOVINS_H__

