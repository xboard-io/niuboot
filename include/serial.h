/*
 * Debug UART register definitions
 */

#ifndef _UART_H
#define _UART_H

#define CONFIG_BAUDRATE 115200
#define CONFIG_DBGUART_CLK  24000000 // 115200 baud


/* UARTDBGFR - Flag Register bits */
#define CTS	0x0001
#define DSR	0x0002
#define DCD	0x0004
#define BUSY	0x0008
#define RXFE	0x0010
#define TXFF	0x0020
#define RXFF	0x0040
#define TXFE	0x0080
#define RI	0x0100

/* UARTDBGLCR_H - Line Control Register bits */
#define BRK	0x0001
#define PEN	0x0002
#define EPS	0x0004
#define STP2	0x0008
#define FEN	0x0010
#define WLEN5	0x0000
#define WLEN6	0x0020
#define WLEN7	0x0040
#define WLEN8	0x0060
#define SPS	0x0080

/* UARTDBGCR - Control Register bits */
#define UARTEN	0x0001
#define LBE	0x0080
#define TXE	0x0100
#define RXE	0x0200
#define DTR	0x0400
#define RTS	0x0800
#define OUT1	0x1000
#define OUT2	0x2000
#define RTSEN	0x4000
#define CTSEN	0x8000

void serial_puts(const char *s);
void serial_init(void);
int  serial_getc(void);
void serial_putc(const char c);
void serial_puthex(unsigned int c);
int serial_tstc (void);

#endif 
