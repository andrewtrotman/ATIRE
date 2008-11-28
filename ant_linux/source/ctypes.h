/*
	CTYPES.H
	--------
	Used to replace the microsoft version that are not inlined!s
	Modified from Linux.
*/
#ifndef __CTYPES_H__
#define __CTYPES_H__

#define _U      0x01    /* upper */
#define _L      0x02    /* lower */
#define _D      0x04    /* digit */
#define _C      0x08    /* cntrl */
#define _P      0x10    /* punct */
#define _S      0x20    /* white space (space/lf/tab) */
#define _X      0x40    /* hex digit */
#define _SP     0x80    /* hard space (0x20) */

extern unsigned char ANT_ctype[];
extern unsigned char ANT_toupper_list[], ANT_tolower_list[];

inline int ANT_ismask(int x) { return (ANT_ctype[(unsigned char)(x)]); }
inline int ANT_isalnum(int c) { return ((ANT_ismask(c) & (_U | _L | _D)) != 0); }
inline int ANT_isalpha(int c) { return ((ANT_ismask(c) & (_U | _L)) != 0); }
inline int ANT_iscntrl(int c) { return ((ANT_ismask(c) & (_C)) != 0); }
inline int ANT_isdigit(int c) { return ((ANT_ismask(c) & (_D)) != 0); }
inline int ANT_isgraph(int c) { return ((ANT_ismask(c) & (_P | _U | _L | _D)) != 0); }
inline int ANT_islower(int c) { return ((ANT_ismask(c) & (_L)) != 0); }
inline int ANT_isprint(int c) { return ((ANT_ismask(c) & (_P | _U | _L | _D | _SP)) != 0); }
inline int ANT_ispunct(int c) { return ((ANT_ismask(c) & (_P)) != 0); }
inline int ANT_isspace(int c) { return ((ANT_ismask(c) & (_S)) != 0); }
inline int ANT_isupper(int c) { return ((ANT_ismask(c) & (_U)) != 0); }
inline int ANT_isxdigit(int c){ return ((ANT_ismask(c) & (_D |_X)) != 0); }
inline int ANT_isascii(int c) { return (((unsigned char)(c))<=0x7f); }

inline unsigned char ANT_toascii(int c) {return (((unsigned char)(c))&0x7f); }
inline unsigned char ANT_tolower(unsigned char c) { return ANT_tolower_list[c]; }
inline unsigned char ANT_toupper(unsigned char c) { return ANT_toupper_list[c]; }

#endif /* __CTYPES_H__ */
