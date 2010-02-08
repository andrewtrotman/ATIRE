/*
	VERSION.C
	---------
*/

#include <stdio.h>

#if defined(_WIN64) || (__WORDSIZE == 64) || (defined(__APPLE__) && (_LP64 == 1))
	#define BITNESS "64"
#elif defined(_WIN32) || (__WORDSIZE == 32) || defined(__APPLE__) 
	#define BITNESS "32"
#else
	#define BITNESS "Unknown"
#endif

#if defined(DEBUG) || defined(_DEBUG)
	#define DEBUGNESS " DEBUGGING-build"
#else
	#define DEBUGNESS ""
#endif

#ifdef PURIFY
	#define PURIFYNESS " PURIFY"
#else
	#define PURIFYNESS ""
#endif

long ANT_version = 0x0003;		// version number in BCD
char *ANT_version_string = "ANT Version 0.3 alpha (" BITNESS "-bit" DEBUGNESS PURIFYNESS")\nWritten (w) 2008-2010 Andrew Trotman, University of Otago";


/*
	ANT_CREDITS()
	-------------
*/
void ANT_credits(void)
{
puts(ANT_version_string);
puts("");

puts("DESIGN & IMPLEMENTATION");
puts("-----------------------");
puts("Andrew Trotman");
puts("");

puts("MANAGEMENT");
puts("----------");
puts("Andrew Trotman");
puts("Shlomo Geva");
puts("");

puts("HASHING ALGORITHMS");
puts("------------------");
puts("Random Hash             : Andrew Trotman");
puts("ANT Hash                : Andrew Trotman");
puts("");

puts("SORTING ALGORITHMS");
puts("------------------");
puts("qsort (top-k addapted)  : J.L. Bentley, M.D. McIlroy");
puts("");

puts("FILE MANIPULATION");
puts("-----------------");
puts("Windows                 : Andrew Trotman");
puts("MacOS                   : Vaughn Wood");
puts("Linux                   : Vaughn Wood");
puts("");

puts("COMPRESSION");
puts("-----------");
puts("Variable Byte           : Andrew Trotman");
puts("Simple-9                : Vikram Subramanya, Andrew Trotman");
puts("Relative-10             : Vikram Subramanya, Andrew Trotman");
puts("Carryover-12            : Vo Ngoc Anh, Alistair Moffat  (http://www.cs.mu.oz.au/~alistair/carry/)");
puts("Sigma-Encoding          : Vikram Subramanya, Andrew Trotman");
puts("Elias Delta             : Andrew Trotman");
puts("Elias Gamma             : Andrew Trotman");
puts("Golomb                  : Andrew Trotman");
#ifdef ANT_HAS_ZLIB
	puts("ZLIB                    : Jean-loup Gailly and Mark Adler (http://www.zlib.net/)");
#endif
#ifdef ANT_HAS_BZLIB
	puts("BZLIB                   : Julian Seward (http://www.bzip.org/)");
#endif
puts("");

puts("STEMMERS");
puts("--------");
puts("Paice Husk              : Andrew Stark (http://www.scientificpsychic.com/paice/paice.c)");
puts("Lovins                  : Linh Huynh, Eibe Frank (http://www.cs.waikato.ac.nz/~eibe/stemmers/index.html)");
puts("Otago                   : Vaughn Wood");
puts("Porter                  : Andrew Trotman");
puts("S-stripper              : Andrew Trotman");
puts("");

puts("LANGUAGE FEATURES");
puts("-----------------");
puts("UTF-8 parser            : Eric Tang");
puts("East Asian Segmentation : Eric Tang");
puts("Dale Chall Readability  : Matt Crane");
}
