/*
	CTYPES.H
	--------
	Used to replace the microsoft version that are not inlined!s
	Modified from Linux.
*/
#ifndef __CTYPES_H__
#define __CTYPES_H__

#define _ANT_U       0x01    /* upper */
#define _ANT_L       0x02    /* lower */
#define _ANT_D       0x04    /* digit */
#define _ANT_C       0x08    /* cntrl */
#define _ANT_P       0x10    /* punct */
#define _ANT_S       0x20    /* white space (space/lf/tab) */
#define _ANT_X       0x40    /* hex digit */
#define _ANT_SP      0x80    /* hard space (0x20) */
#define _ANT_VO     0x100	  /* Vowel */
#define _ANT_Y      0x200	  /* Y */

extern unsigned short ANT_ctype[];
extern unsigned char ANT_toupper_list[], ANT_tolower_list[];

inline int ANT_ismask(int x) { return (ANT_ctype[(unsigned char)(x)]); }
inline int ANT_isalnum(int c) { return ((ANT_ismask(c) & (_ANT_U | _ANT_L | _ANT_D)) != 0); }
inline int ANT_isalpha(int c) { return ((ANT_ismask(c) & (_ANT_U | _ANT_L)) != 0); }
inline int ANT_iscntrl(int c) { return ((ANT_ismask(c) & (_ANT_C)) != 0); }
inline int ANT_isdigit(int c) { return ((ANT_ismask(c) & (_ANT_D)) != 0); }
inline int ANT_isgraph(int c) { return ((ANT_ismask(c) & (_ANT_P | _ANT_U | _ANT_L | _ANT_D)) != 0); }
inline int ANT_islower(int c) { return ((ANT_ismask(c) & (_ANT_L)) != 0); }
inline int ANT_isprint(int c) { return ((ANT_ismask(c) & (_ANT_P | _ANT_U | _ANT_L | _ANT_D | _ANT_SP)) != 0); }
inline int ANT_ispunct(int c) { return ((ANT_ismask(c) & (_ANT_P)) != 0); }
inline int ANT_isspace(int c) { return ((ANT_ismask(c) & (_ANT_S)) != 0); }
inline int ANT_isupper(int c) { return ((ANT_ismask(c) & (_ANT_U)) != 0); }
inline int ANT_isxdigit(int c){ return ((ANT_ismask(c) & (_ANT_D |_ANT_X)) != 0); }
inline int ANT_isascii(int c) { return (((unsigned char)(c))<=0x7f); }

inline int ANT_isvowel(int c) { return ((ANT_ismask(c) & (_ANT_VO)) != 0); }
inline int ANT_isvowely(int c) { return ((ANT_ismask(c) & (_ANT_VO | _ANT_Y)) != 0); }

inline unsigned char ANT_toascii(int c) {return (((unsigned char)(c))&0x7f); }
inline unsigned char ANT_tolower(unsigned char c) { return ANT_tolower_list[c]; }
inline unsigned char ANT_toupper(unsigned char c) { return ANT_toupper_list[c]; }

#endif __ANT_CTYPES_H__
