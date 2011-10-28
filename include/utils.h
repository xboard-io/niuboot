#ifndef __UTILS_H__
#define __UTILS_H__

#include "types.h"

char *puts(const char* s);
char *gets(char *s);
int putchar(const int c);
int getchar(void);
int printf(const char* format, ...);
int strlen(const char *s);
char *strcpy(char *s1, const char *s2);
char *strcat( char *dst, char *src);
int strncmp(const char *cs, const char *ct, size_t count);
int strcmp(const char *cs, const char *ct);
void *memcpy(void *s1, const void *s2, size_t n);
void *memset(void *s1, int c, int n);
int memcmp(const void *cs, const void *ct, size_t count);

void sys_reboot();

#define TOLOWER(x) ((x) | 0x20)
#define _U  0x01    /* upper */
#define _L  0x02    /* lower */
#define _D  0x04    /* digit */
#define _C  0x08    /* cntrl */
#define _P  0x10    /* punct */
#define _S  0x20    /* white space (space/lf/tab) */
#define _X  0x40    /* hex digit */
#define _SP 0x80    /* hard space (0x20) */
#define __ismask(x) (_ctype[(int)(unsigned char)(x)])
#define isxdigit(c) ((__ismask(c)&(_D|_X)) != 0)
#define isdigit(c)  ((__ismask(c)&(_D)) != 0)
extern unsigned char _ctype[];
unsigned long simple_strtoul(const char *cp, char **endp, unsigned int base);

#endif /* __UTILS_H__ */
