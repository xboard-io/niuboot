#include "utils.h"
#include "serial.h"
#include "types.h"
#include <stdarg.h>

int strlen(const char *s)
{
	const char *start = s;

	while (*s)
		s++;

	return s - start;
}

char *strcpy(char *s1, const char *s2)
{
	char *s = s1;

	while ((*s1++ = *s2++) != '\0')
		;

	return s;
}

char *strcat( char *dst, char *src)
{
	char *s = dst + strlen(dst);
	strcpy(s, src);

	return dst; 
}

int strncmp(const char *cs, const char *ct, size_t count)
{
	signed char __res;
	size_t n;

	n = 0;
	__res = 0;
	while (n < count) {
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
		n++;
	}
	return __res;
}

int strcmp(const char *cs, const char *ct)
{
	signed char __res;

	while (1) {
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
	}
	return __res;
}

void *memcpy(void *s1, const void *s2, size_t n)
{
	char *dst = s1;
	const char *src = s2;

	while (n--)
		*dst++ = *src++;

	return s1;
}

void *memset(void *s1, int c, int n)
{
	char *dst = s1;

	while (n--)
		*dst++ = c & 0xff;

	return s1;
}

int memcmp(const void *cs, const void *ct, size_t count)
{
	const unsigned char *su1, *su2;
	int res = 0;

	for (su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
		if ((res = *su1 - *su2) != 0)
			break;
	return res;
}

unsigned char _ctype[] = {
    _C,_C,_C,_C,_C,_C,_C,_C,            /* 0-7 */
    _C,_C|_S,_C|_S,_C|_S,_C|_S,_C|_S,_C,_C,     /* 8-15 */
    _C,_C,_C,_C,_C,_C,_C,_C,            /* 16-23 */
    _C,_C,_C,_C,_C,_C,_C,_C,            /* 24-31 */
    _S|_SP,_P,_P,_P,_P,_P,_P,_P,            /* 32-39 */
    _P,_P,_P,_P,_P,_P,_P,_P,            /* 40-47 */
    _D,_D,_D,_D,_D,_D,_D,_D,            /* 48-55 */
    _D,_D,_P,_P,_P,_P,_P,_P,            /* 56-63 */
    _P,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U,  /* 64-71 */
    _U,_U,_U,_U,_U,_U,_U,_U,            /* 72-79 */
    _U,_U,_U,_U,_U,_U,_U,_U,            /* 80-87 */
    _U,_U,_U,_P,_P,_P,_P,_P,            /* 88-95 */
    _P,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L,  /* 96-103 */
    _L,_L,_L,_L,_L,_L,_L,_L,            /* 104-111 */
    _L,_L,_L,_L,_L,_L,_L,_L,            /* 112-119 */
    _L,_L,_L,_P,_P,_P,_P,_C,            /* 120-127 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        /* 128-143 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        /* 144-159 */
    _S|_SP,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,   /* 160-175 */
    _P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,       /* 176-191 */
    _U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,       /* 192-207 */
    _U,_U,_U,_U,_U,_U,_U,_P,_U,_U,_U,_U,_U,_U,_U,_L,       /* 208-223 */
    _L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,       /* 224-239 */
    _L,_L,_L,_L,_L,_L,_L,_P,_L,_L,_L,_L,_L,_L,_L,_L};      /* 240-255 */

unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base)
{
    unsigned long result = 0,value;

    if (!base) {
        base = 10;
        if (*cp == '0') {
            base = 8;
            cp++;
            if ((TOLOWER(*cp) == 'x') && isxdigit(cp[1])) {
                cp++;
                base = 16;
            }
        }
    } else if (base == 16) {
        if (cp[0] == '0' && TOLOWER(cp[1]) == 'x')
            cp += 2;
    }
    while (isxdigit(*cp) &&
           (value = isdigit(*cp) ? *cp-'0' : TOLOWER(*cp)-'a'+10) < base) {
        result = result*base + value;
        cp++;
    }
    if (endp)
        *endp = (char *)cp;
    return result;
}

char *itox(unsigned int num)
{
	static char hex[12] = "0x";
	int i, pos;
	const char lut[]="0123456789ABCDEF";
	
	for(i = 0, pos = 2; i < 8; i++) {
		if( (hex[pos] = lut[ (num<<4*i)>>28 ]) != '0' ||  pos != 2 )
			pos++;
		hex[pos+1]='\0';
	}	
	return hex;
}

int printf(const char *format, ...)
{
	int i;
	va_list arg_list;
	va_start( arg_list, format );

	for( i=0;format[i];i++ )
	{
		if(format[i] == '%')
		{
			switch(format[++i])
			{
				case 's':
					puts( va_arg( arg_list, char *) ); 
					continue;
				case 'x':
					puts( itox( va_arg( arg_list, unsigned int ) ) );
					continue;
				default:
					i--;
			}		
		}
		putchar(format[i]);
	}
	return i;
}

char *puts(const char *s)
{
	char *no_standard_return =(char*) s;
	while (*s)
		putchar(*s++);

	return no_standard_return;
}

char *gets(char *s)
{
	return 0;
}

int putchar(int c)
{
	serial_putc(c);
	return c;
}

int getchar(void)
{
	return serial_getc();
}

void sys_reboot()
{
	/* Reset the digital sections of the chip, but not the power modules. */
	/* HW_CLKCTRL_RESET_WR(1); */
}
