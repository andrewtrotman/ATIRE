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

#ifdef IMPACT_HEADER
long ANT_version = 0x0004;		// version number in BCD
char *ANT_version_string = "ATIRE Version 0.4 alpha (" BITNESS "-bit" DEBUGNESS PURIFYNESS")\nWritten (w) 2008-2012 Andrew Trotman, University of Otago";
#else
long ANT_version = 0x0003;		// version number in BCD
char *ANT_version_string = "ATIRE Version 0.3 alpha (" BITNESS "-bit" DEBUGNESS PURIFYNESS")\nWritten (w) 2008-2012 Andrew Trotman, University of Otago";
#endif



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

puts("STANDARD ALGORITHMS");
puts("-------------------");
puts("heap  (heap-k addapted) : Fei Jia");
puts("qsort (top-k addapted)  : J.L. Bentley, M.D. McIlroy");
puts("Mersenne Twister        : Takuji Nishimura, Makoto Matsumoto");
puts("");

puts("FILE MANIPULATION");
puts("-----------------");
puts("Windows                 : Andrew Trotman");
puts("MacOS                   : Vaughn Wood");
puts("Linux                   : Vaughn Wood");
puts("Pregen Creation         : Nick Sherlock");
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
#ifdef ANT_HAS_LZO
	puts("LZO                     : Markus F.X.J. Oberhumer (http://www.oberhumer.com/)");
#endif
puts("");

puts("STEMMERS");
puts("--------");
puts("Double Metaphone        : Adam J. Nelson, Lawrence Phillips (http://www.apocryph.org/metaphone/)");
puts("Krovetz                 : Lemur port of Bob Krovetz's original (http://lexicalresearch.com/)");
puts("Otago (all versions)    : Vaughn Wood");
#ifdef ANT_HAS_PAICE_HUSK
	puts("Paice Husk              : Andrew Stark (http://www.scientificpsychic.com/paice/paice.c)");
#endif
puts("Porter (ATIRE's verison): Andrew Trotman");
puts("S-stripper              : Andrew Trotman");
#ifdef ANT_HAS_SNOWBALL
	puts("Snowball stemmers       : Martin Porter et al. (http://snowball.tartarus.org/)");
#endif
puts("");

puts("LANGUAGE FEATURES");
puts("-----------------");
puts("UTF-8 parser            : Eric Tang, Andrew Trotman, Nick Sherlock");
puts("East Asian Segmentation : Eric Tang");
puts("Dale Chall Readability  : Matt Crane");
puts("Wordnet 3.0             : Princeton University");
puts("Query Expansion         : Andrew Trotman");
puts("");

#if defined (ANT_HAS_MYSQL) || defined (ANT_HAS_XPDF)
puts("PLUGINS");
puts("-------");
#ifdef ANT_HAS_MYSQL
	puts("MySQL Connection        : http://www.mysql.com/");
#endif
#ifdef ANT_HAS_XPDF
	puts("xpdf PDF Connection     : http://www.foolabs.com/xpdf/");
#endif
puts("");
#endif

puts("None of whome endorse this product!");
}
