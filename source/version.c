/*
	VERSION.C
	---------
*/
#if defined(_WIN64) || (__SIZEOF_POINTER__ == 8)
	#define BITNESS "64"
#elif defined(_WIN32) || (__SIZEOF_POINTER__ == 4)
	#define BITNESS "32"
#else
	#define BITNESS "Unknown"
#endif
long ANT_version = 0x0001;		// version number in BCD
char *ANT_version_string = "ANT Version 0.1 alpha (" BITNESS "-bit)\nWritten (w) 2008, 2009 Andrew Trotman, University of Otago";

