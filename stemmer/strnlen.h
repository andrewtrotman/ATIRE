#ifndef __STRNLEN_H__
#define __STRNLEN_H__

/* only for platforms without strnlen() */
#ifdef __APPLE__

#include <stddef.h>
inline size_t strnlen(char *s, size_t n) {
    size_t l = 0;
    while (l < n && s[l])
        l++;
    return l;
}

#endif /* __APPLE__ */

#endif /* __STRNLEN_H__ */
