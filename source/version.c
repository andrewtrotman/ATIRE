/*
	VERSION.C
	---------
*/

#if defined(_WIN64) || (__SIZEOF_POINTER__ == 8) || (defined(__APPLE__) && (_LP64 == 1))
	#define BITNESS "64"
#elif defined(_WIN32) || (__SIZEOF_POINTER__ == 4) || defined(__APPLE__) 
	#define BITNESS "32"
#else
	#define BITNESS "Unknown"
#endif

long ANT_version = 0x0003;		// version number in BCD
char *ANT_version_string = "ANT Version 0.3 alpha (" BITNESS "-bit)\nWritten (w) 2008, 2009 Andrew Trotman, University of Otago";

