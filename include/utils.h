#ifndef __UTILS_H__
#define __UTILS_H__

/* minimal implementation of string functions */
/*
static char *strstr(const char *s1, const char *s2)
{
	int i;

	if (*s1 == '\0')
		return *s2 ? 0 : (char *)s1;

	while (*s1) {
		for (i = 0; ; i++) {
			if (s2[i] == '\0')
				return (char *)s1;
			if (s2[i] != s1[i])
				break;
		}
		s1++;
	}
	return 0;
}
*/
char* puts( const char* s );
char* gets( char *s );
int putchar( const int c );
int getchar( void );

int strlen(const char *s);
char *strcpy(char *s1, const char *s2);
char * strcat( char *dst, char *src);
int strncmp(const char *str1, const char *str2, int n);
int strcmp(const char *str1, const char *str2);
void udelay(int usecs);
void *memcpy(void *s1, const void *s2, int n);
void sys_reboot();
void mdelay(int s);

#define NULL 0

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
unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base);

#endif //__UTILS_H__
